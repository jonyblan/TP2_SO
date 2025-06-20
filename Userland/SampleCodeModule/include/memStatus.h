#ifndef MEMSTATUS_H
#define MEMSTATUS_H

#include <stdint.h>

typedef struct {
    uint32_t total;   /* bytes totales administrados por el backend */
    uint32_t used;    /* bytes hoy ocupados                         */
    uint32_t free;    /* total-used                                 */
} MemStatus;

#define SYS_MEM_STATUS  40

#endif