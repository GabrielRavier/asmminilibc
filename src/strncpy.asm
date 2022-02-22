    section .text

    extern memset
    global strncpy:function (strncpy.end - strncpy)
    align 16
strncpy:
    push rbx

    mov rbx, rdi
    test rdx, rdx
    je .ret_rbx

    xor eax, eax
    jmp .start_loop

    align 16

.loop:
    cmp rax, rdx
    je .ret_rbx

.start_loop:
    movzx ecx, byte [rsi + rax]
    inc rax
    mov [rdi], cl
    inc rdi
    test cl, cl
    jne .loop

    cmp rax, rdx
    jnb .ret_rbx

    sub rdx, rax
    xor esi, esi
    call memset wrt ..plt

.ret_rbx:
    mov rax, rbx

    pop rbx
    ret
.end:
