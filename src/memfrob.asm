    section .text

    global memfrob:function (memfrob.end - memfrob)
    align 16
memfrob:
    ;; Return value
    mov rax, rdi

    test rsi, rsi
    je .ret

    xor ecx, ecx

    align 16
.loop:
    xor byte [rax + rcx], 42
    inc rcx
    cmp rsi, rcx
    jne .loop

.ret:
    ret

.end:
