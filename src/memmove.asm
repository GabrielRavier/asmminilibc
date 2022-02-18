    section .text

    global memmove:function (memmove.end - memmove)
    global memcpy:function (memmove.end - memmove)

    ;; Just use the same function for memmove and memcpy, the very slight gains from not dealing with overlapping source/destination are outweighed by the effort required to make the memcpy version.

    ;; When the size is less than 128 bytes, we load all of the source at once into the XMM registers and store them together, which makes things quick (and avoids any overlap problems)
    ;; This uses overlapping loads and stores to minimize branching as much as possible

    ;; When the size is less than 2 KiB, we just use SSE2 (available on all x86-64 processors)
    ;; If we have destination <= source, we do a forwards copy of 4 XMMs at once, with unaligned loads but aligned stores
    ;; We loads the first 4 XMMs and the very last one before the loop and store them after the loop so that overlapping addresses work properly
    ;; Otherwise, we do the exact same thing except the copy goes backwards

    ;; When the size is between 2 KiB and 0.75 MiB, we use rep movsb, which usually has better performance than SSE2 for those sizes on modern processors

    ;; When the size is higher than 0.75 MiB, we are starting to hit cache limits (Note: 0.75 MiB is 3/4ths of typical cache sizes on modern processors). Use non-temporal stores in that case. However, we do not do this if there is overlap between the destination and the source.
    ;; We also take the opportunity to copy 2 pages at once (4 pages at once if we're copying more than 12 MiB) if possible when the source and destination to not page alias
    align 16
memcpy:
memmove:
    ;; Return value
    mov rax, rdi

    cmp rdx, 0xF
    jbe .less_1_xmm

    cmp rdx, 0x20
    ja .more_2_xmm

.between_1_xmm_2_xmm:
    ;; Size is between 0x10 and 0x20. This deliberately is as inclusive as possible (i.e. takes sizes of 0x10 and 0x20) so that there is no branching at all for those cases
    movups xmm0, [rsi]
    movups xmm1, [rsi - 0x10 + rdx]
    movups [rdi], xmm0
    movups [rdi - 0x10 + rdx], xmm1

.ret:
    ret

    align 16
.less_1_xmm:
    cmp rdx, 7
    ja .between_8_15

    cmp dl, 3
    ja .between_4_7

    cmp dl, 1
    ja .between_2_3

    test dl, dl
    je .ret

    movzx r8d, BYTE [rsi]
    mov [rdi], r8b
    ret

    align 16
.between_8_15:
    mov r8, [rsi]
    mov rcx, [rsi - 8 + rdx]
    mov [rdi - 8 + rdx], rcx
    mov [rdi], r8
    ret

    align 16
.between_4_7:
    mov ecx, [rsi - 4 + rdx]
    mov r8d, [rsi]
    mov [rdi - 4 + rdx], ecx
    mov [rdi], r8d
    ret

    align 16
.between_2_3:
    movzx ecx, word [rsi - 2 + rdx]
    movzx r8d, word [rsi]
    mov [rdi - 2 + rdx], cx
    mov [rdi], r8w
    ret

    align 16
.more_2_xmm:
    ;; We use rdx later. This also allows us to avoid this mov later when doing rep movsb
    mov rcx, rdx

    ;; Above 0x800, we don't do a simple SSE loop anymore (the check for >0.75 MiB is there)
    cmp rdx, 0x800
    ja .movsb

    cmp rdx, 0x80
    ja .more_8_xmm

    ;; Size is from 2 XMMs to 8 XMMs
    ;; Do the first 4 loads here as we know we're gonna do them anyway
    movups xmm0, [rsi]
    movups xmm1, [rsi + 0x10]
    movups xmm2, [rsi - 0x10 + rcx]
    movups xmm3, [rsi - 0x20 + rcx]

    cmp rcx, 64
    jbe .between_2_4_xmm

    ;; 4 XMM to 8 XMM
    movups xmm4, [rsi + 0x20]
    movups xmm5, [rsi + 0x30]
    movups xmm6, [rsi - 0x30 + rcx]
    movups xmm7, [rsi - 0x40 + rcx]

    movups [rdi], xmm0
    movups [rdi + 0x10], xmm1
    movups [rdi + 0x20], xmm4
    movups [rdi + 0x30], xmm5
    movups [rdi - 0x10 + rcx], xmm2
    movups [rdi - 0x20 + rcx], xmm3
    movups [rdi - 0x30 + rcx], xmm6
    movups [rdi - 0x40 + rcx], xmm7
    ret

    align 16
.movsb:
    ;; Above 0.75 MiB we don't do movsb anymore
    cmp rdx, 0xC000
    ja .non_temporal

    cmp rax, rsi
    jb .do_actual_movsb

    ;; We check for destination == source relatively late considering how uncommon it is, but it is still valuable to avoid doing useless copying
    je .ret

    ;; destination > source. We now check for destination + size < source, so we know if the areas actually overlap and we really need a backwards copy . Since it is very slow, so we don't do it
    lea r8, [rsi + rdx]
    cmp rax, r8
    jb .more_8_xmm_backwards

.do_actual_movsb:
    mov rdi, rax
    rep movsb
    ret

    align 16
.more_8_xmm:
    cmp rsi, rax
    jb .more_8_xmm_backwards

    ;; We check for destination == source relatively late considering how uncommon it is, but it is still valuable to avoid doing useless copying
    je .ret

    ;; Load the first XMM, along with the last 4, now, such as to properly handle overlapping
    movups xmm3, [rsi]
    movups xmm4, [rsi - 0x10 + rcx]
    movups xmm5, [rsi - 0x20 + rcx]
    movups xmm6, [rsi - 0x30 + rcx]
    movups xmm7, [rsi - 0x40 + rcx]

    ;; We save the start and end address of the destination buffer.
    ;; We then align the destination, such that we get to have aligned stores in the loop. We compute how much the destination how much the destination is misaligned, then get the negative of the offset for the alignment, and finally adjust source, destination (which will then be aligned) and length
    mov r9, rax
    lea rdi, [rcx - 0x10]
    mov edx, 0x10
    and r9d, 0xF

    ;; End of actual zone we use for the end stores
    add rdi, rax

    sub rdx, r9
    lea rcx, [r9 - 0x51 + rcx]

    ;; Adjust destination
    lea r8, [rax + rdx]

    ;; Adjust source
    add rdx, rsi

    ;; Adjusted end pointer
    and rcx, -0x40
    lea rcx, [r8 + 0x40 + rcx]

    align 16
.loop_4_xmm_forwards:
    movups xmm8, [rdx]
    movups xmm2, [rdx + 0x10]
    movups xmm1, [rdx + 0x20]
    movups xmm0, [rdx + 0x30]
    add rdx, 0x40

    movaps [r8], xmm8
    movaps [r8 + 0x10], xmm2
    movaps [r8 + 0x20], xmm1
    movaps [r8 + 0x30], xmm0
    add r8, 0x40

    cmp rcx, r8
    jne .loop_4_xmm_forwards

    ;; First XMM
    movups [rax], xmm3

    ;; Last 4
    movups [rdi], xmm4
    movups [rdi - 0x10], xmm5
    movups [rdi - 0x20], xmm6
    movups [rdi - 0x30], xmm7
    ret

    align 16
.more_8_xmm_backwards:
    ;; We load the first 4 XMMs and the last one now to make overlapping addresses work
    movups xmm4, [rsi]
    movups xmm5, [rsi + 0x10]
    movups xmm6, [rsi + 0x20]
    movups xmm7, [rsi + 0x30]
    movups xmm8, [rsi + rcx - 0x10]

    ;; Saved end of the destination buffer
    lea r8, [rsi + rcx - 0x10]

    ;; Aligns destination end (for aligned stores), first by computing how misaligned it is and then applying the offset to the source, destination and length
    lea rdi, [rax + rcx - 0x10]
    sub rcx, 0x41
    mov r9, rdi
    and r9d, 0xF
    sub r8, r9
    sub rcx, r9
    mov rdx, r8

    ;; Aligned source, from which we derive the rest
    mov r8, rdi
    and rcx, -0x40
    and r8, -0x10

    ;; Calculate end pointer ("end" here meaning where iteration ends)
    lea rsi, [r8 - 0x40]
    sub rsi, rcx

    ;; Distance between source and destination, from which we calculate the source pointer every time
    mov rcx, rdx
    sub rcx, r8

    align 16
.loop_4_xmm_backwards:
    lea rdx, [rcx + r8]
    sub r8, 0x40
    movups xmm0, [rdx]
    movups xmm1, [rdx - 0x10]
    movups xmm2, [rdx - 0x20]
    movups xmm3, [rdx - 0x30]
    movaps [r8 + 0x40], xmm0
    movaps [r8 + 0x30], xmm1
    movaps [r8 + 0x20], xmm2
    movaps [r8 + 0x10], xmm3
    cmp rsi, r8
    jne .loop_4_xmm_backwards

    ;; First 4 XMM
    movups [rax], xmm4
    movups [rax + 0x10], xmm5
    movups [rax + 0x20], xmm6
    movups [rax + 0x30], xmm7

    ;; Last XMM
    movups [rdi], xmm8
    ret

    align 16
.between_2_4_xmm:
    ;; 2 to 4 XMMs (no loading since we already did it before)
    movups [rdi], xmm0
    movups [rdi + 0x10], xmm1
    movups [rdi - 0x10 + rdx], xmm2
    movups [rdi - 0x20 + rcx], xmm3
    ret

    align 16
.non_temporal:
    ;; abs(source - destination)
    mov rdx, rdi
    sub rdx, rsi
    mov rdi, rdx
    mov r8, rdx
    sar rdi, 0x3F
    xor r8, rdi
    sub r8, rdi

    ;; If there is overlap, we can't use non-temporal stores: in that case, the destination might be in cache while the source is loaded...
    cmp r8, rcx
    jb .more_8_xmm

    ;; Align the destination to a cache boundary. To help with this, start by storing the first 64 bytes
    movups xmm0, [rsi]
    movups xmm1, [rsi + 0x10]
    movups xmm2, [rsi + 0x20]
    movups xmm3, [rsi + 0x30]

    push r12
    push rbp
    push rbx

    movups [rax], xmm0
    movups [rax + 0x10], xmm1
    movups [rax + 0x20], xmm2
    movups [rax + 0x30], xmm3

    ;; Align the pointers
    mov r8, rax
    and r8d, 0x3F
    mov r9d, 0x40
    sub r9, r8
    dec rdx
    lea r8, [r8 - 0x40 + rcx]
    add rsi, r9
    add r9, rax
    mov ebp, r8d

    ;; This tests whether the source and destination address will page alias (0xF80 is the page size - 8 XMMs)
    ;; If it will, use the version for 4 pages to alleviate the drop in performance
    test edx, 0xF80
    je .non_temporal_4_pages

    ;; Also do it if the copying is REALLY big (>=12 MiB)
    cmp r8, 0xBFFFFF
    ja .non_temporal_4_pages

    ;; Reminder size for copying the tail after doing all the pages
    and ebp, 0x1FFF

    ;; Get a proper size that's aligned to the page
    mov rbx, ~0xFFFFFFFF
    lea rdi, [r9 + 0x1000]
    and rbx, r8
    mov edx, ebp
    and r8, ~0x1FFF
    or rbx, rdx
    mov r12, r8

    ;; End pointer for outer loop
    lea r11, [rsi + r8]
    mov r10, rbx

    align 16
.loop_non_temporal_2_pages_outer:
    ;; Start pointer for inner loop
    lea r8, [rdi - 0x1000]
    lea rcx, [rsi]

    align 16
.loop_non_temporal_2_pages_inner:
    prefetcht0 [rcx + 0x40]
    prefetcht0 [rcx + 0x80]
    prefetcht0 [rcx + 0x1040]
    prefetcht0 [rcx + 0x1080]

    movups xmm0, [rcx]
    movups xmm1, [rcx + 0x10]
    movups xmm2, [rcx + 0x20]
    movups xmm3, [rcx + 0x30]
    movups xmm4, [rcx + 0x1000]
    movups xmm5, [rcx + 0x1010]
    movups xmm6, [rcx + 0x1020]
    movups xmm7, [rcx + 0x1030]
    add rcx, 0x40

    movntdq [r8], xmm0
    movntdq [r8 + 0x10], xmm1
    movntdq [r8 + 0x20], xmm2
    movntdq [r8 + 0x30], xmm3
    movntdq [r8 + 0x1000], xmm4
    movntdq [r8 + 0x1010], xmm5
    movntdq [r8 + 0x1020], xmm6
    movntdq [r8 + 0x1030], xmm7
    add r8, 0x40

    cmp r8, rdi
    jne .loop_non_temporal_2_pages_inner

    add rsi, 0x2000
    lea rdi, [r8 + 0x2000]
    cmp rsi, r11
    jne .loop_non_temporal_2_pages_outer

    lea r8, [r9 + r12]
    sfence

    ;; Are only the last for loads needed ?
    cmp ebp, 0x40
    jbe .non_temporal_2_pages_end

    ;; Get out the remaining size from old calculations
    lea esi, [rbp - 0x41]
    lea rdx, [r11]
    shr esi, 6
    lea edi, [rsi + 1]
    sal rdi, 6
    lea rcx, [r8 + rdi]

    align 16
.loop_non_temporal_2_pages_tail:
    prefetcht0 [r8 + 0x40]
    prefetcht0 [rdx + 0x40]

    movups xmm0, [rdx]
    movups xmm1, [rdx + 0x10]
    movups xmm2, [rdx + 0x20]
    movups xmm3, [rdx + 0x30]
    add rdx, 0x40

    movaps [r8], xmm0
    movaps [r8 + 0x10], xmm1
    movaps [r8 + 0x20], xmm2
    movaps [r8 + 0x30], xmm3
    add r8, 0x40

    cmp r8, rcx
    jne .loop_non_temporal_2_pages_tail

    ;; Get back the proper pointer to do the very last loads and stores from
    sal esi, 6
    mov rdx, ~0xFFFFFFFF
    add r11, rdi
    neg esi
    and rbx, rdx
    lea r10d, [rbp - 0x40 + rsi]
    or r10, rbx

.non_temporal_2_pages_end:
    ;; Store the last 4 XMMs
    movups xmm0, [r11 - 0x40 + r10]
    movups xmm1, [r11 - 0x30 + r10]
    movups xmm2, [r11 - 0x20 + r10]
    movups xmm3, [r11 - 0x10 + r10]
    movups [r8 - 0x40 + r10], xmm0
    movups [r8 - 0x30 + r10], xmm1
    movups [r8 - 0x20 + r10], xmm2
    movups [r8 - 0x10 + r10], xmm3

.non_temporal_ret:
    pop rbx
    pop rbp
    pop r12
    ret

.non_temporal_4_pages:
    ;; Reminder size for copying the tail after doing all the pages
    and ebp, 0x3FFF

    ;; Get a proper size that's aligned to the page
    mov rbx, ~0xFFFFFFFF
    lea rcx, [r9 + 0x1000]
    and rbx, r8
    mov edx, ebp
    and r8, ~0x3FFF
    or rbx, rdx
    mov rdi, r8

    ;; End pointer for outer loop
    lea r11, [rsi + r8]
    mov r10, rbx

    align 16
.loop_non_temporal_4_pages_outer:
    ;; Start pointer for inner loop
    lea rdx, [rcx - 0x1000]
    lea r8, [rsi]

    align 16
.loop_non_temporal_4_pages_inner:
    ;; Unlike for the 2 pages loop, we only do a single prefetch per page, since the longer loop gives the prefetcher more time
    prefetcht0 [r8 + 0x40]
    prefetcht0 [r8 + 0x1040]
    prefetcht0 [r8 + 0x2040]
    prefetcht0 [r8 + 0x3040]

    movups xmm0, [r8 + 0x00]
    movups xmm1, [r8 + 0x10]
    movups xmm2, [r8 + 0x20]
    movups xmm3, [r8 + 0x30]
    movups xmm4, [r8 + 0x1000]
    movups xmm5, [r8 + 0x1010]
    movups xmm6, [r8 + 0x1020]
    movups xmm7, [r8 + 0x1030]
    movups xmm8, [r8 + 0x2000]
    movups xmm9, [r8 + 0x2010]
    movups xmm10, [r8 + 0x2020]
    movups xmm11, [r8 + 0x2030]
    movups xmm12, [r8 + 0x3000]
    movups xmm13, [r8 + 0x3010]
    movups xmm14, [r8 + 0x3020]
    movups xmm15, [r8 + 0x3030]
    add r8, 0x40

    movntdq [rdx + 0x00], xmm0
    movntdq [rdx + 0x10], xmm1
    movntdq [rdx + 0x20], xmm2
    movntdq [rdx + 0x30], xmm3
    movntdq [rdx + 0x1000], xmm4
    movntdq [rdx + 0x1010], xmm5
    movntdq [rdx + 0x1020], xmm6
    movntdq [rdx + 0x1030], xmm7
    movntdq [rdx + 0x2000], xmm8
    movntdq [rdx + 0x2010], xmm9
    movntdq [rdx + 0x2020], xmm10
    movntdq [rdx + 0x2030], xmm11
    movntdq [rdx + 0x3000], xmm12
    movntdq [rdx + 0x3010], xmm13
    movntdq [rdx + 0x3020], xmm14
    movntdq [rdx + 0x3030], xmm15
    add rdx, 0x40

    cmp rdx, rcx
    jne .loop_non_temporal_4_pages_inner

    add rsi, 0x4000
    lea rcx, [rdx + 0x4000]
    cmp rsi, r11
    jne .loop_non_temporal_4_pages_outer

    lea rcx, [r9 + rdi]
    sfence

    ;; Are only the last for loads needed ?
    cmp ebp, 0x40
    jbe .non_temporal_4_pages_end

    ;; Get out the remaining size from old calculations
    lea edi, [rbp - 0x41]
    mov rdx, r11
    lea r8, [rcx]
    shr edi, 6
    mov r9d, edi
    mov rsi, r9
    sal rsi, 6
    lea rsi, [rcx + 0x40 + rsi]

    align 16
.loop_non_temporal_4_pages_tail:
    prefetcht0 [rdx + 0x40]
    prefetcht0 [r8]

    movups xmm0, [rdx + 0x00]
    movups xmm1, [rdx + 0x10]
    movups xmm2, [rdx + 0x20]
    movups xmm3, [rdx + 0x30]
    add rdx, 0x40

    movaps [r8 + 0x00], xmm0
    movaps [r8 + 0x10], xmm1
    movaps [r8 + 0x20], xmm2
    movaps [r8 + 0x30], xmm3
    add r8, 0x40

    cmp r8, rsi
    jne .loop_non_temporal_4_pages_tail

    ;; Get back the proper pointer to do the very last loads and stores from
    lea r8, [r9 + 1]
    sal edi, 6
    mov rdx, -0x100000000
    sal r8, 6
    neg edi
    and rbx, rdx
    add r11, r8
    add rcx, r8
    lea r8d, [rbp - 0x40 + rdi]
    or rbx, r8
    mov r10, rbx

.non_temporal_4_pages_end:
    ;; Store the last 4 XMMs
    movups xmm0, [r11 - 0x40 + r10]
    movups xmm1, [r11 - 0x30 + r10]
    movups xmm2, [r11 - 0x20 + r10]
    movups xmm3, [r11 - 0x10 + r10]

    movups [rcx - 0x40 + r10], xmm0
    movups [rcx - 0x30 + r10], xmm1
    movups [rcx - 0x20 + r10], xmm2
    movups [rcx - 0x10 + r10], xmm3
    jmp .non_temporal_ret
.end:
