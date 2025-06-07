#include <stdint.h>
#include <memoryManager.h>
#include "include/PCBQueueADT.h"
#include <stddef.h>
#include <processManager.h>

#define MAX_PROCESSES 128

typedef struct PCBQueueCDT{
    PCB* queue[MAX_PROCESSES];
    uint8_t head;
    uint8_t tail;
    uint8_t size;
}PCBQueueCDT;

PCBQueueADT CreatePCBQueueADT(){
    PCBQueueADT toReturn = (PCBQueueADT) malloc(sizeof(*toReturn));
    if (toReturn != NULL)
    {  
        toReturn->head=0;
        toReturn->tail=0;
        toReturn->size=0;
    }
    return toReturn;
}

int queueProcess(PCBQueueADT adt, void* process){
    if (adt->size == MAX_PROCESSES) return -1;
    
    adt->size++;
    adt->queue[adt->tail]=process;
    adt->tail= (adt->tail + 1) % MAX_PROCESSES;
}

void* dequeueProcess(PCBQueueADT adt){
    if (adt->size == 0) return NULL;

    void* toReturn =adt->queue[adt->head];
    adt->size--;
    adt->head= (adt->head+1) % MAX_PROCESSES;
    return toReturn;
}

uint8_t getPCBQueueSize(PCBQueueADT adt){
    return adt->size;
}