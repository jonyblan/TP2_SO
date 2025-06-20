#ifndef SYSCALLS_DISPATCHER_H
#define SYSCALLS_DISPATCHER_H

#include <stdint.h>
#include <time_and_rtc.h>
#define SYS_MEM_STATUS 40

typedef enum
{
    STDOUT = 1,
    STDERR,
    STDMARK,
} FileDescriptor;

//uint64_t syscallDispatcher(uint64_t id, ...);
uint64_t syscallDispatcher(uint64_t id, uint64_t arg1, uint64_t arg2, uint64_t arg3);
uint64_t read(FileDescriptor fd, char *buffer, uint64_t count);
uint64_t write(FileDescriptor fd, const char *buffer, uint64_t count);
uint64_t getRegBackup(uint64_t *arr);
uint64_t sys_mem_status(uint64_t rdi, uint64_t _1, uint64_t _2,  uint64_t _3);

void nanoFace();
#endif