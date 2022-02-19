    section .text

    global strrchr:function (strrchr.end - strrchr)
    global rindex:function (strrchr.end - strrchr)
    align 16
rindex:
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
