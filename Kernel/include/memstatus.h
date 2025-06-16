#ifndef MEMSTATUS_H
#define MEMSTATUS_H

#include <stdint.h>

/* Número de syscall reservado para consultar el estado de memoria.
   Si ya elegiste otro, cámbialo aquí y en syscallDispatcher.h          */
#define SYS_MEM_STATUS 40

/* Estructura que el kernel llenará y el user‐space leerá */
typedef struct {
    uint32_t total;   /* bytes de RAM física gestionados por el kernel   */
    uint32_t used;    /* bytes actualmente asignados a procesos/heap     */
    uint32_t free;    /* bytes libres (= total − used)                   */
} MemStatus;

#endif /* MEMSTATUS_H */
