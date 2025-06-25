#include <processManager.h>
#include <stdint.h>
#include <stddef.h>
#include <memoryManager.h>
#include <PCBQueueADT.h>
#include <videoDriver.h>
#include <scheduler.h>
#include <lib.h>
#include <mySem.h>

extern void forceTimerInterruption();

static PCB processes[MAX_PROCESSES];
static PCBQueueADT terminatedProcessesQueue; //Cola de procesos esperando a que le hagan wait() (Si terminan, su PCB se marca como TERMINATED, por lo que se podria pisar el PCB)
PCB* currentProcess;
static uint8_t processCount= 1;
static pid_t nextPID= 1;

int getPriority(pid_t pid){
	if(pid < 0 || pid > MAX_PROCESSES){
		return -1;
	}
	return processes[pid].priority;
}

void setPriority(pid_t pid, int newPriority){
	if(pid < 0 || pid > MAX_PROCESSES){
		return ;
	}
	processes[pid].priority = (newPriority >= PRIORITY_LEVELS) ? PRIORITY_LEVELS - 1 : newPriority;
}

void myExit(){
    uint16_t pid = getCurrentPID();
    killProcess(pid);
}

void launchProcess(void (*fn)(uint8_t, char **), uint8_t argc, char *argv[]) {
  fn(argc, argv);
  myExit();
}


void *stackStart= (void*) 0x1000000;


void initializeProcesses(){
    for (uint16_t i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].state=TERMINATED;
    }
    
}

void prepareStack(PCB* PCB, void* stack,void* entrypoint) {
    processStack *pStack = stack - sizeof(processStack);
    pStack->rsp = stack;
    pStack->rbp = stack;
    pStack->cs = (void *)0x8;
    pStack->rflags = (void *)0x202;
    pStack->ss = 0x0;
    pStack->rip = entrypoint;
    PCB->stackPointer = pStack;
}

void loadArguments(void (*fn)(uint8_t, char **), uint8_t argc, char *argv[],
                    void *stack) {
    processStack *pStack = stack - sizeof(processStack);
    pStack->rdi = fn;
    pStack->rsi = (void *)(uintptr_t)argc; 
    pStack->rdx = argv; 
}
void createFirstProcess(void (*fn)(uint8_t, char **), int argc, char** argv){
    PCB* new= &processes[0];
    new->pid=0;
    new->state=READY;
    new->priority=1;
    new->fd[0] = STDIN; 
    new->fd[1] = STDOUT; 
    new->stackBase = stackStart;
    new->entryPoint=launchProcess;
    char name[]="init";
    memcpy(new->name,name,strlen(name));
    processStack *newStack = new->stackBase - sizeof(processStack);
    newStack->rsp = new->stackBase;
    newStack->rbp = new->stackBase;
    newStack->cs = (void *)0x8; // Kernel code segment
    newStack->rflags = (void *)0x202; // Set interrupt flag
    newStack->ss = 0x0; // Kernel data segment
    newStack->rip = launchProcess; // Entry point for the idle process
    newStack->rdi = fn;
    newStack->rsi = (void *)(uintptr_t) argc; // Argument count
    newStack->rdx = argv; // Argument vector
    new->stackPointer = newStack;
    scheduleProcess(new);
    
    processCount++;
}

pid_t createProcess(void (*fn)(uint8_t, char **), int priority, int argc, char** argv, const char* name){
    if (processCount>= MAX_PROCESSES) return -1;
    PCB* new=NULL;
    int pid= nextPID++;
    
    new = &processes[pid];
    new->pid = pid;
    new->priority = (priority >= PRIORITY_LEVELS) ? PRIORITY_LEVELS - 1 : priority;
    new->foreground = 1;
    new->waitingChildren = 0;
    new->fd[0] = STDIN; 
    new->fd[1] = STDOUT; 
    new->argc = argc;
    new->argv = argv;
    new->entryPoint = launchProcess;
    new->parent = &processes[getCurrentPID()];
    new->next = NULL;

    char **args = malloc(sizeof(char*) * (argc+1));
    for (uint8_t i = 0; i < argc; i++)
    {
        uint64_t len = strlen(argv[i]) + 1;
        args[i] = malloc(len);
        memcpy(args[i], argv[i], len);  
    }
    args[argc] = NULL; 
    new->argv = args;
    memcpy(new->name,name,strlen(name));
    new->stackBase = (stackStart + new->pid * PROCESS_STACK_SIZE);


    
    prepareStack(new, new->stackBase, new->entryPoint);
    loadArguments(fn, argc, argv,new->stackBase);
    new->state = READY;
    scheduleProcess(new);
    
    processCount++;
    
    return new->pid;
}

int blockProcess(uint16_t pid) {
    if (pid >= MAX_PROCESSES) {
        return -1; // Invalid PID or process already terminated
    }
    processes[pid].state = BLOCKED;
    if(getCurrentPID() == pid) {
        yield(); // If the current process is blocked, yield to allow other processes to run
    }
    else {
        // If not the current process, just change state
        descheduleProcess(&processes[pid]);
    }
    return 0;
}

int unblockProcess(uint16_t pid) {
    if (pid >= MAX_PROCESSES) {
        return -1; // Invalid PID or process already terminated
    }
    if (processes[pid].state == BLOCKED) {
        processes[pid].state = READY;
        scheduleProcess(&processes[pid]);
    }
    return 0; // Process unblocked successfully
}

/* PCB* getNextProcess() {
    for (int priority = 0; priority < PRIORITY_LEVELS; priority++) {
        PCBQueueADT queue = processQueues[priority];
        int queueSize = getPCBQueueSize(queue);  

        for (int i = 0; i < queueSize; i++) {
            PCB* candidate = dequeueProcess(queue);  
            if (candidate->state == READY ) {
                queueProcess(queue, candidate);     
                return candidate;
            } else {
                queueProcess(queue, candidate);     
            }
        }
    }
    return &processes[0];
} */


int killProcess(uint8_t pid){
    for (size_t i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == pid)
        {
            if (processes[i].state == TERMINATED) return -1;
            
            processes[i].state = TERMINATED;
            processCount--;
            if(processes[i].parent->waitingChildren){
                sem_post(processes[i].parent->waitSemaphore);
            }
            
            if (getCurrentPID() == pid)
            {
                yield();
            }else{
                descheduleProcess(&processes[i]);
            }
            return 0;
        }
    }
    return -1;
}

void yield(){
    forceTimerInterruption();
}

void showRunningProcesses(){
    for (size_t i = 0; i < MAX_PROCESSES ; i++)
    {
        if (processes[i].state== READY || processes[i].state== RUNNING)
        {
            vdPrint("Process "); vdPrintDec(i); vdPrint(" Is running"); vdPrintChar('\n');
        }
        
    }
    return;
}

void cleanTerminatedList(){
    uint8_t size= getPCBQueueSize(terminatedProcessesQueue);
    PCB* current;
    for (uint8_t i = 0; i < size; i++)
    {
        current= dequeueProcess(terminatedProcessesQueue);
        if (current->waitingChildren)
        {
            queueProcess(terminatedProcessesQueue,current);
        }
        else
        {
            free(current->stackBase);
        }
    }
}

PCB* getPCBByPID(pid_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return NULL; // Invalid PID
    }
    return &processes[pid];
}

void wait(pid_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;

    PCB* child = &processes[pid];
    PCB* parent = getCurrentProcess();

    if (child->parent != parent) return;
    if (child->state == TERMINATED) return;
    
    // Crear semáforo con nombre basado en child's PID
    char semName[12];
    itoa(pid, semName);
    
    parent->waitSemaphore = sem_open(semName, 0);
    parent->waitingChildren = 1;
    
    sem_wait(parent->waitSemaphore);
    sem_destroy(parent->waitSemaphore);
    parent->waitingChildren = 0;
}

uint8_t ps(processInfo *toReturn){
    uint8_t count=0;
    for (uint16_t i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state==RUNNING || processes[i].state==READY || processes[i].state== BLOCKED)
        {
            
            PCB* aux= &processes[i];
            safe_strncpy(toReturn[count].name,aux->name,MAX_NAME_LEN);
            toReturn[count].name[MAX_NAME_LEN-1]='\0';
            toReturn[count].pid=i;
            toReturn[count].priority=aux->priority;
            toReturn[count].state= aux->state;
            toReturn[count].stackBase=  aux->stackBase;
            toReturn[count].stackPointer= aux->stackPointer;
            count++;
        }
    }
    
    return count;
}