#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define PROCESS_MAX_NAME_LEN 16
#define PROCESS_STACK_SIZE 16384 // 16KiB
#define DEFAULT_PRIORITY 50
#define MAX_PROCESSES 256

#include "../include/memoryManager.h"

typedef enum{
	READY = 0,
	RUNNING,
	BLOCKED,
	TERMINATED 
} STATE;

typedef struct Process {
    pid_t pid;
    char name[PROCESS_MAX_NAME_LEN];        // Process name (optional but useful)

    STATE state;                  	// e.g., READY, RUNNING, BLOCKED, TERMINATED
    uint8_t priority;               // For scheduling

    void* stackBase;                // Base of the allocated stack
    void* stackPointer;             // Current stack pointer (for context switching)

    void* entryPoint;               // The function to run (e.g., main of the process)

    struct Process* parent;         // For wait() logic
    struct Process* next;           // For scheduling queue

    uint8_t foreground;                 // 1 = foreground, 0 = background
    uint8_t waitingChildren;            // Track if wait() is needed

	uint8_t argc;
	char** argv;
}PCB;

typedef struct processStack{
    void *r15;
    void *r14;
    void *r13;
    void *r12;
    void *r11;
    void *r10;
    void *r9;
    void *r8;
    void *rsi;
    void *rdi;
    void *rbp;
    void *rdx;
    void *rcx;
    void *rbx;
    void *rax;
    void *rip;
    void *cs;
    void *rflags;
    void *rsp;
    void *ss;
}processStack;

extern PCB* currentProcess;

pid_t createProcess(void (*fn)(uint8_t, char **), int priority, int argc, char** argv);

int initializeProcesses();

void terminateProcess();

int killProcess(uint8_t pid);

void quantumTick(); //funcion para manejar los quantums

void showRunningProcesses();

void createFirstProcess(void (*fn)(uint8_t, char **), int argc, char** argv);

void yield();
#endif