
#include <lib.h>
#include <processManager.h>
#include <PCBQueueADT.h>
#include <scheduler.h>
#include <mySem.h>
#define SEM_NAME_MATCH(name1, name2) (strcmp((name1), (name2)) == 0)

extern void acquireLock(int8_t *lock);
extern void releaseLock(int8_t *lock);

static spinlock_t createLock = 0;
NamedSemaphore namedSemaphores[MAX_SEMAPHORES];

uint8_t sem_init(Semaphore* sem, uint8_t initial_value) {
    if (!sem || initial_value < 0)
        return -1;

    sem->value = initial_value;
    sem->lock = 0; 
    sem->waiters = CreatePCBQueueADT();  // Or zero/init static queue
    if (!sem->waiters) return -1;

    return 0;
}


uint8_t sem_open(const char* name, uint8_t initial_value) {
    if (!name) return -1;

    acquireLock(&createLock);
    // First, search for existing
    for (uint8_t i = 0; i < 5; i++) {
        if (namedSemaphores[i].inUse && SEM_NAME_MATCH(namedSemaphores[i].name, name)) {
            releaseLock(&createLock);
            return i;
        }
    }

    // Create new semaphore
    for (uint8_t i = 0; i < MAX_SEMAPHORES; i++) {
        if (!namedSemaphores[i].inUse) {
            namedSemaphores[i].inUse = 1;
            safe_strncpy(namedSemaphores[i].name, name, MAX_NAME_LEN);
            sem_init(&namedSemaphores[i].sem, initial_value);
            releaseLock(&createLock);
            return i;
        }
    }

    releaseLock(&createLock);
    return -1;  // No free slots
}

int sem_post(uint8_t id) {
    if (id < 0 || id >= MAX_SEMAPHORES || !namedSemaphores[id].inUse){
        return -1;
	}

    Semaphore* sem = &namedSemaphores[id].sem;
    
    acquireLock(&sem->lock);

    if (getPCBQueueSize(sem->waiters) > 0) {
        PCB* p = dequeueProcess(sem->waiters);
        unblockProcess(p->pid); 
    }
    sem->value++;
    
    releaseLock(&sem->lock);
	return sem->value;

}


int sem_wait(uint8_t id) {
    if (id < 0 || id >= MAX_SEMAPHORES || !namedSemaphores[id].inUse){
        return -1;
	}

    Semaphore* sem = &namedSemaphores[id].sem;
    acquireLock(&sem->lock);
    if (sem->value > 0) {
        sem->value--;
    }
	else {
        PCB* current = getCurrentProcess();
        queueProcess(sem->waiters, current);
        releaseLock(&sem->lock);
        blockProcess(current->pid);
        yield();  // Voluntary context switch
        return sem->value;
    }

    releaseLock(&sem->lock);
	return sem->value;
}

void sem_destroy(uint8_t id){
	namedSemaphores[id].inUse = 0;
}







