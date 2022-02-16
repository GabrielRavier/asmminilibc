    section .text

    global strchr:function (strchr.end - strchr)
    align 16
strchr:
    mov rax, rdi

    ;; Loop until we find the first instance or the null terminator, with finding taking priority, such that '\0' can be searched with this function
    align 16
.loop:
    movzx ecx, byte [rax]
    cmp cl, sil
    je .ret

    inc rax
    test cl, cl
    jne .loop

    ;; Could not find character
    xor eax, eax

.ret:
    ret
.end:

