    section .text

    extern strlen
    extern strncmp

    global strstr:function (strstr.end - strstr)
    align 16
strstr:
    ;; Check if needle is an empty string (we need to return haystack immediately in that case)
    ;; We also re-use the assignment to rax that we do here as an iterator through the haystack
    mov r8b, [rsi]
    mov rax, rdi
    test r8b, r8b
    jne .start_loop
    jmp .ret

    align 16
.outer_loop:
    test cl, cl
    je .ret

.next_haystack_char:
    inc rax
    
.start_loop:
    mov dl, [rax]
    test dl, dl
    je .ret_0

    ;; Quick check for the first character from the needle
    cmp dl, r8b
    jne .next_haystack_char

    ;; Loop through the rest of the attempted match, starting at index 1 since we already checked the first character
    mov edx, 1

    align 16
.inner_loop:
    movzx ecx, byte [rsi + rdx]
    test cl, cl
    je .ret

    cmp cl, [rax + rdx]
    lea rdx, [rdx + 1]
    je .inner_loop
    jmp .outer_loop

    align 16
.ret_0:
    xor eax, eax

.ret:
    ret
.end:
