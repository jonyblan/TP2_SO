#include <semaphore.h>
#include <lib.h>
#include <processManager.h>
#include <scheduler.h>
#define SEM_NAME_MATCH(name1, name2) (strcmp((name1), (name2), MAX_NAME_LEN) == 0)

/*
NamedSemaphore namedSemaphores[MAX_SEMAPHORES];

int sem_init(Semaphore* sem, int initial_value) {
    if (!sem || initial_value < 0)
        return -1;

    sem->value = initial_value;
    sem->waiters = createPCBQueue();  // Or zero/init static queue
    if (!sem->waiters) return -1;

    return 0;
}


int sem_open(const char* name, int create, int initial_value) {
    if (!name) return -1;

    // First, search for existing
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (namedSemaphores[i].inUse && SEM_NAME_MATCH(namedSemaphores[i].name, name)) {
            return i;
        }
    }

    // If not found and create is false, fail
    if (!create) return -1;

    // Create new semaphore
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!namedSemaphores[i].inUse) {
            namedSemaphores[i].inUse = 1;
            strncpy(namedSemaphores[i].name, name, MAX_NAME_LEN);
            sem_init(&namedSemaphores[i].sem, initial_value);
            return i;
        }
    }

    return -1;  // No free slots
}


int sem_init(int initialValue){
	if (sem_id < 0 || sem_id >= MAX_SEMAPHORES || initial_value < 0)
        return -1;

    Semaphore* sem = &semaphores[sem_id];
    sem->value = initial_value;

    if (sem->waiters == NULL)
        sem->waiters = createPCBQueue();

    if (!sem->waiters)
        return -1;

    return 0;
}

void sem_post(int id) {
    if (id < 0 || id >= MAX_SEMAPHORES || !namedSemaphores[id].inUse)
        return;

    Semaphore* sem = &namedSemaphores[id].sem;
    cli();

    if (sem->waiters->size > 0) {
        PCB* p = dequeuePCB(sem->waiters);
        unblockProcess(p);  // Make it READY again
    } else {
        sem->value++;
    }

    sti();
}


void sem_wait(int id) {
    if (id < 0 || id >= MAX_SEMAPHORES || !namedSemaphores[id].inUse)
        return;

    Semaphore* sem = &namedSemaphores[id].sem;
    cli();

    if (sem->value > 0) {
        sem->value--;
    } else {
        PCB* current = getCurrentProcess();
        current->state = BLOCKED;
        enqueuePCB(sem->waiters, current);
        sti();
        yield();  // Voluntary context switch
        return;
    }

    sti();
}
*/