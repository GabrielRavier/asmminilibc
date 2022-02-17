    section .text

    global strcmp:function (strcmp.end - strcmp)
    align 16
strcmp:
    ;; Check first byte
    mov al, [rdi]
    mov dl, [rsi]
    cmp al, dl
    jne .retSub

    mov ecx, 1

.loop:
    ;; If we find the terminator in one string before the other, the cmp will have caught the difference
    test al, al
    je .ret0

    movzx eax, byte [rdi + rcx]
    movzx edx, byte [rsi + rcx]
    inc rcx
    cmp al, dl
    je .loop

    ;; Simplest way of returning < 0 for a < b and > 0 for a > b
    ;; Note that the operation is unsigned, as the standard mandates
.retSub:
    movzx eax, al
    movzx ecx, dl
    sub eax, ecx
    ret

.ret0:
    xor eax, eax
    ret
.end:
