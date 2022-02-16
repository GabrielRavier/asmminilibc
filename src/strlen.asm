    section .text

    global strlen:function (strlen.end - strlen)
    align 16
strlen:
    mov rax, -1                 ; Initialize the result to -1 so writing the loop is simpler

    align 16
.loop:
    inc rax
    cmp byte [rdi + rax], 0
    jne .loop

    ret
.end:
