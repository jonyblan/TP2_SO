#include <processManager.h>
#include <stdint.h>
#include <stddef.h>
#include <memoryManager.h>
#include "include/PCBQueueADT.h"

extern void* prepareStack(int argc, char **argv, void *stack, void *entryPoint);
extern void idle();

static PCB processes[MAX_PROCESSES];
static PCBQueueADT processQueues[PRIORITY_LEVELS];
static PCBQueueADT terminatedProcessesQueue; //Cola de procesos esperando a que le hagan wait() (Si terminan, su PCB se marca como TERMINATED, por lo que se podria pisar el PCB)
PCB* currentProcess;
static uint8_t processCount= 1;
static pid_t nextPID= 1;
static uint8_t initialized = 0;

int initializeProcesses(){
    if (initialized) return 0;

    //Si el estado del PCB esta en TERMINATED, esta disponible
    for (size_t i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].state= TERMINATED;
    }
    
    
    //Iniciar colas de las diferentes priorities y los procesos terminados a espera de wait
    for (size_t i = 0; i < PRIORITY_LEVELS; i++)
    {   
        processQueues[i] = CreatePCBQueueADT();
        if (processQueues[i] == NULL) return -1;
    }

    terminatedProcessesQueue= CreatePCBQueueADT();
    if (terminatedProcessesQueue == NULL) return -1;

    //Inicializar y poner en ejecucion el proceso idle
    PCB* idlePCB= &processes[0];
    idlePCB->pid=0;
    idlePCB->state=READY;
    idlePCB->priority=0;
    idlePCB->stackBase= malloc(PROCESS_STACK_SIZE);
    idlePCB->entryPoint=idle;
    if (idlePCB->stackBase == NULL) return 1;
    idlePCB->stackPointer=prepareStack(0, NULL, idlePCB->stackBase + PROCESS_STACK_SIZE - 0x08, idlePCB->entryPoint);

    currentProcess= idlePCB;
    initialized=1;
    return 0;
}

pid_t createProcess(void* entryPoint, int priority, int argc, char** argv){
    if (processCount>= MAX_PROCESSES) return -1;
    PCB* new=NULL;

    for (size_t i = 0; i < MAX_PROCESSES; i++)
    {
       if (processes[i].state == TERMINATED || processes[i].pid == 0) {
            new = &processes[i];
            break;
        }
    }
    
    if (new == NULL) return -1;
    new->pid = nextPID++;
    new->priority = (priority >= PRIORITY_LEVELS) ? PRIORITY_LEVELS - 1 : priority;
    new->state = READY;
    new->foreground = 1;
    new->waitingChildren = 0;
    new->argc = argc;
    new->argv = argv;
    new->entryPoint = entryPoint;
    new->parent = currentProcess;
    new->next = NULL;
    new->stackBase=malloc(PROCESS_STACK_SIZE);
    if (new->stackBase==NULL) return -1;

    new->stackPointer=prepareStack(new->argc,new->argv, new->stackBase + PROCESS_STACK_SIZE - 0x08, new->entryPoint);

    queueProcess(processQueues[priority], new);
    
    processCount++;
    
    return new->pid;
}

PCB* getNextProcess() {
    for (int priority = 0; priority < PRIORITY_LEVELS; priority++) {
        PCBQueueADT queue = processQueues[priority];
        int queueSize = getADTSize(queue);  

        for (int i = 0; i < queueSize; i++) {
            PCB* candidate = dequeueProcess(queue);  
            if (candidate->state == READY) {
                queueProcess(queue, candidate);     
                return candidate;
            } else {
                queueProcess(queue, candidate);     
            }
        }
    }
    
    return &processes[0];
}

void scheduler(){
    //(TO DO) Guardar contexto del proceso actual

    PCB* next= getNextProcess();

    if (currentProcess->state=RUNNING)
    {
        currentProcess->state=READY;
        queueProcess(processQueues[currentProcess->priority],currentProcess);
    }
    
    next->state=RUNNING;
    currentProcess= next;

    //(TO DO) cargar el contexto de next
}

/* #include <processManager.h>
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
 */