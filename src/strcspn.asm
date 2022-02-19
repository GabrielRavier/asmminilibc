    section .text

    global strcspn:function (strcspn.end - strcspn)
    align 16
strcspn:
    mov r9b, [rdi]
    mov rax, rdi
    test r9b, r9b
    je .ret_rax_min_rdi

    mov r8b, [rsi]
    test r8b, r8b
    je .empty_charset

    inc rsi
    mov rax, rdi                ; ???

    align 16
.outer_loop:
    ;; Charset iterator
    mov rdx, rsi

    ;; First charset character, to make things slightly faster
    mov ecx, r8d

    align 16
.inner_loop:
    cmp cl, r9b
    je .ret_rax_min_rdi

    movzx ecx, byte [rdx]
    inc rdx
    test cl, cl
    jne .inner_loop

    inc rax
    mov r9b, [rax]
    test r9b, r9b
    jne .outer_loop
    jmp .ret_rax_min_rdi

    ;; This is basically just strlen, since there is nothing to search for
.empty_charset:
    mov rcx, rdi

    align 16
.empty_charset_loop:
    inc rcx
    cmp byte [rcx], 0
    jne .empty_charset_loop
    
    mov rax, rcx

.ret_rax_min_rdi:
    sub rax, rdi
    ret
.end:
