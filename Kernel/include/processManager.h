#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define PROCESS_MAX_NAME_LEN 16
#define PROCESS_STACK_SIZE 16384 // 16KiB
#define PRIORITY_LEVELS 2
#define DEFAULT_PRIORITY 50
#define MAX_PROCESSES 128

#include <memoryManager.h>

typedef enum{
	READY = 0,
	RUNNING,
	BLOCKED,
	TERMINATED 
} State;

typedef struct ProcessControlBlock {
    pid_t pid;
    char name[PROCESS_MAX_NAME_LEN];        // Process name (optional but useful)

    State state;                  	// e.g., READY, RUNNING, BLOCKED, TERMINATED
    uint8_t priority;               // For scheduling

    void* stackBase;                // Base of the allocated stack
    void* stackPointer;             // Current stack pointer (for context switching)

    void* entryPoint;               // The function to run (e.g., main of the process)

    struct PCB* parent;         // For wait() logic
    struct PCB* next;           // For scheduling queue

    uint8_t foreground;                 // 1 = foreground, 0 = background
    uint8_t waitingChildren;            // Track if wait() is needed

	uint8_t argc;
	char** argv;
}PCB;

pid_t createProcess(void* entryPoint, int priority, int argc, char** argv);

#endif