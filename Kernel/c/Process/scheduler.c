#include <scheduler.h>
#include <stdint.h>
#include <stddef.h>
#include <PCBQueueADT.h>
#include <processManager.h>
#include <memoryManager.h>
#include <time_and_rtc.h>

static PCB* idlePCB;
static int idlePID;

/* void idleProcess(uint8_t argc, char **argv) {
    while (1)
        _hlt();  // O haltcpu(), si la tenés definida
} */

schedulerStruct *scheduler;

void initScheduler(void *stackBase) {
  scheduler = malloc(sizeof(schedulerStruct));
  if(scheduler == NULL) {
    vdPrint("Error al inicializar el scheduler: malloc fallido");
    sleep(1);
    return;
  }
  for (uint8_t i = 0; i < MAX_PRIO; i++) {
    scheduler->schedule[i] = CreatePCBQueueADT();
  }

  scheduler->currentRunningPCB = malloc(sizeof(PCB));
  scheduler->currentRunningPCB->state= TERMINATED;
  scheduler->currentRunningPCB->stackBase = stackBase;
  for (uint8_t i = 0; i < MAX_PRIO; i++) {
      scheduler->count[i] = 0;
  }
  /* char *argv[] = { 0 };
  idlePID= createProcess((void*)idleProcess,0,0,argv);
  idlePCB= getPCBByPID(idlePID);  */
}

void scheduleProcess(PCB *pcb) {
  queueProcess(scheduler->schedule[pcb->priority], pcb);
}

void descheduleProcess(PCB *pcb) {
  removeProcess(scheduler->schedule[pcb->priority], pcb);
}

void *schedule(void *rsp) {
   scheduler->currentRunningPCB->stackPointer = rsp;
  if (scheduler->count[0] == 1) {
    for (uint8_t i = 0; i < MAX_PRIO; i++) {
      scheduler->count[i] = 0;
    }
  }
  
  if (scheduler->currentRunningPCB->state == READY ) {
    queueProcess(scheduler->schedule[scheduler->currentRunningPCB->priority], scheduler->currentRunningPCB);
  }

  for (int8_t i = MAX_PRIO - 1; i >= 0; i--) {
    if (!isEmpty(scheduler->schedule[i]) && scheduler->count[i] <= (i * 5 + 1)) {
      scheduler->currentRunningPCB = dequeueProcess(scheduler->schedule[i]);
      (scheduler->count[i])++;
      break;
    }
  }
  return scheduler->currentRunningPCB->stackPointer;
}

uint16_t getCurrentPID(){return scheduler->currentRunningPCB->pid;}
PCB* getCurrentProcess(){return scheduler->currentRunningPCB;}