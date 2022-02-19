    section .text

    global ffs:function (ffs.end - ffs)
    align 16
ffs:
    bsf eax, edi
    mov edx, -1
    cmove eax, edx
    inc eax
    ret
.end:
