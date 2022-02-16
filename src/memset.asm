    section .text

    global memset:function (memset.end - memset)
    align 16
memset:
    ;;  Return value
    mov rax, rdi

    test rdx, rdx
    je .ret

    xor ecx, ecx

    align 16
.loop:
    mov [rax + rcx], sil
    inc rcx
    cmp rdx, rcx
    jne .loop

.ret:
    ret 
.end:
