; loadProcess.asm

global loadProcessAsm

section .text

loadProcessAsm:
    mov rsp, rdi         ; rdi = stack pointer

    pop rbp              ; fake RBP
    mov rdi, 0           ; argc = 0
    mov rsi, 0           ; argv = NULL

    call [rsp]           ; call entryPoint
    ret                  ; return back to caller
