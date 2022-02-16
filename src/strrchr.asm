    section .text

    global strrchr:function (strrchr.end - strchr)
    align 16
strrchr:
    ;; Initialize return value with NULL, i.e. nothing found
    xor eax, eax

    ;; Loop until null terminator, setting the return value each time we find the wanted result (including the last character)
    align 16
.loop:
    movzx ecx, byte [rdi]
    cmp cl, sil
    cmove rax, rdi

    inc rdi
    test cl, cl
    jne .loop

    ret
.end:
