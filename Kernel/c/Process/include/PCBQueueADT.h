#ifndef _PCB_QUEUE_ADT_H
#define _PCB_QUEUE_ADT_H

typedef struct PCBQueueCDT *PCBQueueADT;

PCBQueueADT CreatePCBQueueADT();

int queueProcess(PCBQueueADT adt, void* process);

void* dequeueProcess(PCBQueueADT adt);

uint8_t getPCBQueueSize(PCBQueueADT adt);

#endif