    section .text

    global strpbrk:function (strpbrk.end - strpbrk)
    align 16
strpbrk:
    mov r8b, [rsi]
    test r8b, r8b
    je .ret_0

    inc rsi

    ;; Iterator through the searched string
    mov rax, rdi

    align 16
.outer_loop:
    ;; Get first character now to make iteration easier
    mov dl, [rax]

    ;; Iterator through the search set
    mov rdi, rsi
    mov ecx, r8d

    align 16
.inner_loop:
    cmp cl, dl
    je .ret

    movzx ecx, byte [rdi]
    inc rdi
    test cl, cl
    jne .inner_loop

    inc rax

    ;; Stop at end of searched string
    test dl, dl
    jne .outer_loop

.ret_0:
    xor eax, eax

.ret:
    ret
.end:
