    section .text

    global syscall:function (syscall.end - syscall)
    align 16
syscall:
    ;; Syscall number
    mov rax, rdi

    ;; Shift the 5 first arguments to the corresponding registers
    mov rdi, rsi
    mov rsi, rdx
    mov rdx, rcx

    ;; lol slightly different ABI
    mov r10, r8
    mov r8, r9

    ;; The sixth argument is stored on the stack, as mandated by ABI
    mov r9, [rsp + 8]

    ;; Actually do the system call
    syscall

    ;; Awkwardly enough we can't set errno... So I guess we'll just return (if rax > (u64)-4096 then errno = -rax)
    ret
.end:
