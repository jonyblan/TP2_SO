; loadProcess.asm

global contextSwitch
global loadProcessAsm
;global prepareStack
global idle
global firstContextSwitch

%macro initializeStack 0
    
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
    ; Frame de interrupción esperado por IRETQ (orden: RSP → FLAGS → CS → RIP)
    ;push rdx
    push rcx       ; ← RIP → la función a ejecutar
    push 0x08      ; ← CS (código segment selector: típico para kernel code)
    push 0x202     ; ← RFLAGS (habilita interrupciones)

%endmacro

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

section .text

loadProcessAsm:
    mov rsp, rdi     ; Load stack pointer from first argument
    pop rbp          ; Restore base pointer
    ret              ; Return to entryPoint

;prepareStack:
;    mov r8, rsp ; Uso r8 como backup del stack actual

;   mov rsp, rdx ; Set the stack pointer to the new stack

;    initializeStack

;    mov rax, rsp ; Retorno el nuevo rsp

;    mov rsp, r8

;    ret

idle: 
    cli
    hlt
    jmp idle

; rdi = currentRsp  → dónde guardar el rsp actual del proceso que sale
; rsi = nextRsp   → nuevo rsp del proceso que entra
contextSwitch:
    pushState

    mov rdi, rsp
    
    mov rsp, rsi

    popState

    iretq



firstContextSwitch:
    mov rsp, rdi        ; rdi = nuevo stack pointer
    iretq               ; salto directo usando el stack ya armado por prepareStack