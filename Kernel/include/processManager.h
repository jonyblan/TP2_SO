#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define PROCESS_MAX_NAME_LEN 16
#define PROCESS_STACK_SIZE 16384 // 16KiB
#define DEFAULT_PRIORITY 50
#include <memoryManager.h>

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
	char* argv[];
} Process;

uint64_t createProcess(void* entryPoint);
void enqueueProcess(Process* process);

#endif