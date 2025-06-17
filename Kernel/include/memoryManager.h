#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

void  init_heap(void); 
void   *kmalloc (size_t size);
void    kfree   (void *ptr);

size_t  km_total(void);   /* bytes administrados   */
size_t  km_used (void);   /* bytes actualmente en uso */

#endif /* MEMORY_MANAGER_H */