    section .text

    global ffs:function (ffs.end - ffs)
    align 16
ffs:
    ;; Count low bits in edi
    bsf eax, edi
    mov edx, -1

    ;; If edi was 0, the result is -1 (+ 1)
    cmove eax, edx
    inc eax
    ret
.end:
