    section .text

    global strcmp:function (strcmp.end - strcmp)

    ;; We detect different characters by using pcmpeqb and pmovmskb. The reason we do the pminub and second pcmpeqb is to handle the null terminator properly
    align 16
strcmp:
    ;; Check whether one of the pointers is very cose to a page boundary. In that case, reading too far ahead could result in a fault. Use a byte loop in that case
    mov eax, esi
    or eax, edi
    and eax, 0xFFF
    cmp eax, 0xFC1
    jnb .cross_page

.do_first_xmm:
    ;; Fast handling for 1st XMM
    movdqu xmm0, [rdi]
    movdqu xmm1, [rsi]
    pcmpeqb xmm1, xmm0
    pminub xmm1, xmm0

    pxor xmm0, xmm0
    pcmpeqb xmm1, xmm0
    pmovmskb eax, xmm1
    test eax, eax
    jne .xmm_found_match

.do_second_to_fourth_xmm:
    ;; Fast handling for 2nd to 4th XMMs
    movdqu xmm1, [rdi + 0x10]
    movdqu xmm2, [rsi + 0x10]
    movdqu xmm3, [rdi + 0x20]
    movdqu xmm4, [rsi + 0x20]
    movdqu xmm5, [rdi + 0x30]
    movdqu xmm6, [rsi + 0x30]
    
    pcmpeqb xmm2, xmm1
    pcmpeqb xmm4, xmm3
    pcmpeqb xmm6, xmm5

    pminub xmm2, xmm1
    pminub xmm4, xmm3
    pminub xmm6, xmm5

    pcmpeqb xmm2, xmm0
    pcmpeqb xmm4, xmm0
    pcmpeqb xmm6, xmm0

    pmovmskb eax, xmm2
    pmovmskb ecx, xmm4

    shl rax, 0x10
    shl rcx, 0x20
    or rcx, rax

    pmovmskb eax, xmm6

    shl rax, 0x30
    or rax, rcx
    je .start_main_loop
    
.xmm_found_match:
    bsf rax, rax
    add rdi, rax
    add rsi, rax
    jmp .fetch_sub_ret

    align 16
.start_main_loop:
    ;; We align one of the strings, and do this whole shebang so that we can handle crossing pages properly (so that we don't end up faulting on a string ending just before the end of a page)
    pxor xmm8, xmm8
    mov r8d, 0x1000

    lea rax, [rdi + 0x40]

    ;; Align loads from one of the strings
    and rax, -0x40

    ;; Align to page size
    mov ecx, eax
    sub ecx, edi
    add ecx, esi
    and ecx, 0xFFF
    sub r8, rcx

    sub rsi, rdi
    mov ecx, esi
    and ecx, 0x3F

    ;; Amount of blocks of 4 XMMs we can go through before crossing a page
    shr r8, 6

    ;; Offset for when we cross pages
    mov r9, rcx
    neg r9

    ;; Aligned pointer
    lea r10, [rsi + rax]

    test r8, r8
    je .main_loop_cross_page

    align 16
.main_loop_iter_start:
    dec r8
    jmp .main_loop

    align 16
.main_loop_cross_page:
    ;; Slightly elaborate loop to check exactly only the bytes located right before the page ends
    movdqa xmm0, [r10 + r9]
    movdqu xmm1, [rax + r9]
    movdqa xmm2, [r10 + r9 + 0x10]
    movdqu xmm3, [rax + r9 + 0x10]
    movdqa xmm4, [r10 + r9 + 0x20]
    movdqu xmm5, [rax + r9 + 0x20]
    movdqa xmm6, [r10 + r9 + 0x30]
    movdqu xmm7, [rax + r9 + 0x30]

    pcmpeqb xmm0, xmm1
    pcmpeqb xmm2, xmm3
    pcmpeqb xmm4, xmm5
    pcmpeqb xmm6, xmm7

    pminub xmm0, xmm1
    pminub xmm2, xmm3
    pminub xmm4, xmm5
    pminub xmm6, xmm7

    pcmpeqb xmm0, xmm8
    pcmpeqb xmm2, xmm8
    pcmpeqb xmm4, xmm8
    pcmpeqb xmm6, xmm8

    pmovmskb r8d, xmm0
    pmovmskb edi, xmm2
    pmovmskb edx, xmm4

    shl rdi, 0x10
    shl rdx, 0x20
    or r8, rdx
    or r8, rdi

    pmovmskb edi, xmm6
    shl rdi, 0x30
    or r8, rdi

    shr r8, cl
    test r8, r8
    jne .bsf_add_fetch_sub_ret

    mov r8d, 0x3F

    align 16
.main_loop:
    ;; Main loop. This compares 4 XMMs per iteration
    ;; We combine all the result XMMs here to do a single check (doing it for the cross page and would take more valuable code space and would save much less time for obvious reasons), and then actually do it properly if we find that we do have a match
    movdqu xmm0, [r10]
    movdqa xmm1, [rax]
    movdqu xmm2, [r10 + 0x10]
    movdqa xmm3, [rax + 0x10]
    movdqu xmm4, [r10 + 0x20]
    movdqa xmm5, [rax + 0x20]
    movdqu xmm6, [r10 + 0x30]
    movdqa xmm7, [rax + 0x30]
    
    pcmpeqb xmm0, xmm1
    pcmpeqb xmm2, xmm3
    pcmpeqb xmm4, xmm5
    pcmpeqb xmm6, xmm7

    pminub xmm0, xmm1
    pminub xmm2, xmm3
    pminub xmm4, xmm5
    pminub xmm6, xmm7

    movdqa xmm1, xmm0
    movdqa xmm7, xmm4

    pminub xmm1, xmm2
    pminub xmm7, xmm6
    pminub xmm7, xmm1
    pcmpeqb xmm7, xmm8
    pmovmskb edi, xmm7
    test edi, edi
    jne .main_loop_found_something

.continue_main_loop:
    add rax, 0x40
    lea r10, [rsi + rax]
    test r8, r8
    jne .main_loop_iter_start
    jmp .main_loop_cross_page

    align 16
.cross_page_ret_null_term:
    xor edx, edx
    
.cross_page_ret_sub:
    movzx eax, dl
    movzx ecx, cl
    sub eax, ecx
    ret

    ;; *Now* we do the checking properly
.main_loop_found_something:
    pcmpeqb xmm0, xmm8
    pcmpeqb xmm2, xmm8
    pcmpeqb xmm4, xmm8
    pcmpeqb xmm6, xmm8
    
    pmovmskb r8d, xmm0
    pmovmskb ecx, xmm2
    pmovmskb esi, xmm4

    shl rcx, 0x10
    shl rsi, 0x20
    or r8, rcx
    or r8, rsi

    pmovmskb ecx, xmm6
    shl rcx, 0x30
    or r8, rcx

.bsf_add_fetch_sub_ret:
    bsf rcx, r8
    
.add_fetch_sub_ret:
    add rax, rcx
    add r10, rcx
    mov rsi, r10
    mov rdi, rax
    
.fetch_sub_ret:
    mov dl, [rdi]
    mov cl, [rsi]
    movzx eax, dl
    movzx ecx, cl
    sub eax, ecx
    ret

    align 16
.cross_page:
    xor eax, eax

    align 16
.cross_page_loop:
    movzx edx, byte [rdi + rax]
    movzx ecx, byte [rsi + rax]
    test dl, dl
    je .cross_page_ret_null_term

    cmp dl, cl
    jne .cross_page_ret_sub

    inc rax
    cmp rax, 0x40
    jne .cross_page_loop
    jmp .start_main_loop
.end:
