; loadProcess.asm

global loadProcessAsm
global prepareStack
global idle

%macro initializeStack 0
    ; Frame de interrupción esperado por IRETQ (orden: RSP → FLAGS → CS → RIP)
    push rdx       ; ← Valor que se restaurará como RSP
    push 0x202     ; ← RFLAGS (habilita interrupciones)
    push 0x08      ; ← CS (código segment selector: típico para kernel code)
    push rcx       ; ← RIP → la función a ejecutar

    ; Simulación de push_state de todos los registros
    push 0x00      ; RAX
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi       ; argc
    push rsi       ; argv
    push 0x00      ; R8
    push 0x00      ; R9
    push 0x00      ; R10
    push 0x00      ; R11
    push 0x00      ; R12
    push 0x00      ; R13
    push 0x00      ; R14
    push 0x00      ; R15
%endmacro

section .text

loadProcessAsm:
    mov rsp, rdi     ; Load stack pointer from first argument
    pop rbp          ; Restore base pointer
    ret              ; Return to entryPoint

prepareStack:
    mov r8, rsp ; Uso r8 como backup del stack actual

    mov rsp, rdx ; Set the stack pointer to the new stack

    initializeStack

    mov rax, rsp ; Retorno el nuevo rsp

    mov rsp, r8

    ret

idle:
    cli
    hlt
    jmp idle