    section .text

    global strncmp:function (strncmp.end - strncmp)
    align 16
strncmp:
    ;; Put 0 as result rn to make it easier to return that
    xor eax, eax
    xor ecx, ecx

    align 16
 .loop:
    cmp rdx, rcx
    je .ret

    movzx r8d, byte [rdi + rcx]
    movzx r9d, byte [rsi + rcx]
    cmp r8b, r9b
    jne .ret_sub

    inc rcx
    test r8d, r8d
    jne .loop
    jmp .ret

.ret_sub:
    sub r8d, r9d
    mov eax, r8d

.ret:
    ret
.end:
