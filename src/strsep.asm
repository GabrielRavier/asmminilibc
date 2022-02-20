    section .text

    global strsep:function (strsep.end - strsep)
    align 16
strsep:
    ;; Return NULL if *rdi is NULL
    mov rax, [rdi]
    test rax, rax
    je .ret

    mov r9, rax

.outer_loop:
    mov r10b, [r9]

    ;; Delimiters iterator
    mov rdx, rsi

    align 16
.inner_loop:
    movzx ecx, byte [rdx]
    cmp cl, r10b
    je .got_match

    inc rdx
    test cl, cl
    jne .inner_loop

    inc r9
    jmp .outer_loop

.got_match:
    test r10b, r10b
    je .ret_store_null

    ;; End the token
    mov byte [r9], 0

    ;; *rdi must point past the '\0' we wrote
    lea r8, [r9 + 1]
    jmp .ret_store_r8

.ret_store_null:
    xor r8d, r8d

.ret_store_r8:
    mov [rdi], r8

.ret:
    ret    
.end:
