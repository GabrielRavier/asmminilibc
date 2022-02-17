    section .text

    global memcpy:function (memcpy.end - memcpy)
    align 16
memcpy:
    ;; Return value
    mov rax, rdi

    test rdx, rdx
    je .ret

    xor ecx, ecx

    align 16
.loop:
    movzx r8d, byte [rsi + rcx]
    mov [rax + rcx], r8b
    inc rcx
    cmp rcx, rdx
    jne .loop

.ret:
    ret
.end:
