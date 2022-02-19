    section .text

    global strspn:function (strspn.end - strspn)
    align 16
strspn:
    mov r9b, [rdi]
    mov rax, rdi
    test r9b, r9b
    je .ret_rax_min_rdi

    mov r8b, [rsi]
    test r8b, r8b
    je .ret_rax_min_rdi

    inc rsi

    align 16
.outer_loop:
    ;; Charset iterator
    mov rdx, rsi

    ;; First charset character, to make things slightly faster
    mov ecx, r8d

    align 16
.inner_loop:
    cmp cl, r9b
    je .finish_inner

    movzx ecx, byte [rdx]
    inc rdx
    test cl, cl
    jne .inner_loop
    jmp .ret_rax_min_rdi

.finish_inner:
    inc rax
    mov r9b, [rax]
    test r9b, r9b
    jne .outer_loop
    
.ret_rax_min_rdi:
    sub rax, rdi
    ret
.end:
