#include <processManager.h>
#include <stddef.h>
#include <videoDriver.h>
#include <syscallDispatcher.h>

// This would actually "start" the process by switching context to it
extern void loadProcessAsm(Process* process);

static void terminateProcess() {
    // Loop forever for now, since there's no scheduler to switch away
    while (1) {}
}

// For now, we just support one process
static Process* firstProcess = NULL;

void enqueueProcess(Process* process) {
    firstProcess = process;
}

Process* allocateProcessStruct(uint8_t argc) {
    // Allocate space for Process + argv array
    Process* newProcess = (Process*)malloc(sizeof(Process) + sizeof(char*) * argc);
    return newProcess;
}

pid_t generatePid() {
    return 1; // Basic version that always returns 1
}

char* strcpy(char* dest, const char* src) {
    char* original = dest;
    while ((*dest++ = *src++) != '\0');
    return original;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void* setupStack(Process* p) {
    uint64_t* stack = (uint64_t*)((uint8_t*)p->stackBase + PROCESS_STACK_SIZE);

    // Stack grows downward: push values in reverse call order

    // Push fake return address (if the function returns, go to terminateProcess)
    *(--stack) = (uint64_t)terminateProcess;

    // Push arguments as expected by your calling convention
    *(--stack) = (uint64_t)p->argc;
    *(--stack) = (uint64_t)p->argv;

    // Push the "return address" for the entryPoint, simulating a call
    *(--stack) = (uint64_t)p->entryPoint;

    // Push base pointer (fake rbp)
    *(--stack) = 0;

    return (void*)stack;
}

pid_t create(void* entryPoint, uint8_t priority, uint8_t foreground, uint8_t argc, char* argv[]) {
    Process* newProcess = allocateProcessStruct(argc);
    newProcess->pid = generatePid();

    // Copy argv pointers (strings can stay in caller's memory for now)
    for (int i = 0; i < argc; i++) {
    	size_t len = strlen(argv[i]) + 1;
    	newProcess->argv[i] = malloc(len);
    	strcpy(newProcess->argv[i], argv[i]);
	}

	newProcess->argc = argc;

    newProcess->entryPoint = entryPoint;
    newProcess->priority = priority;
    newProcess->foreground = foreground;
    newProcess->state = READY;

    strcpy(newProcess->name, argv[0]);

    newProcess->stackBase = malloc(PROCESS_STACK_SIZE);
    newProcess->stackPointer = setupStack(newProcess);

    enqueueProcess(newProcess);
    return newProcess->pid;
}

pid_t createProcess(void* entryPoint) {
	int argc = 2;
	char* argv[] = {"simpleProcess\0", NULL};
    pid_t pid = create(entryPoint, DEFAULT_PRIORITY, 1, argc, argv);
    loadProcessAsm(firstProcess);
    return pid;
}
