    section .text

    extern strspn
    extern strpbrk

    global strtok_r:function (strtok_r.end - strtok_r)
    align 16
strtok_r:
    push r15
    push r14
    push rbx

    mov r15, rsi
    mov r14, rdx
    mov rbx, rdi

    ;; Is this the first or a subsequent call ?
    test rdi, rdi
    jne .after_save_ptr_fetch

    ;; If there are no tokens left at all, return
    mov rbx, [r14]
    test rbx, rbx
    je .ret_null

.after_save_ptr_fetch:
    ;; Skip leading separators
    mov rdi, rbx
    mov rsi, r15
    call strspn wrt ..plt

    ;; Return if there are no more tokens
    cmp byte [rbx + rax], 0
    je .ret_null

    ;; Go past the token
    add rbx, rax
    mov rsi, r15
    mov rdi, rbx
    call strpbrk wrt ..plt

    test rax, rax
    je .ret_store_null

    mov byte [rax], 0
    inc rax
    jmp .ret_store_rax

.ret_null:
    xor ebx, ebx
    jmp .ret_rbx

.ret_store_null:
    xor eax, eax

.ret_store_rax:
    mov [r14], rax

.ret_rbx:
    mov rax, rbx
    pop rbx
    pop r14
    pop r15
    ret
.end:
