; loadProcess.asm

global loadProcessAsm

section .text

loadProcessAsm:
    mov rsp, rdi     ; Load stack pointer from first argument
    pop rbp          ; Restore base pointer
    ret              ; Return to entryPoint