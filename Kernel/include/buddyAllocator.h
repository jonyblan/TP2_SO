#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>


/* Reserva -al estilo malloc- usando el buddy-system 64 B…1 MiB */
void   *buddy_alloc(size_t size);

/* Libera un bloque previamente reservado con buddy_alloc           */
void    buddy_free (void *ptr);

/* Estadísticas: total administrado, bytes hoy en uso, dump texto   */
size_t  buddy_total(void);                  /* siempre 1 MiB */
size_t  buddy_used (void);
void    buddy_stats(char *buf, size_t len); /* escribe estado de listas */

#endif