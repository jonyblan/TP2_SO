#ifndef MEMSTATUS_H
#define MEMSTATUS_H
#include <stdint.h>
typedef struct {
    uint32_t total;
    uint32_t used;
    uint32_t free;
} MemStatus;
#define SYS_MEM_STATUS 40
#endif