#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H



#define MAX_PID 32768
#include <stdint.h>
#include <stddef.h>

typedef uint64_t pid_t;

void* malloc(uint64_t size);
void free(void* ptr);
void  init_heap(void); 
void   *kmalloc (size_t size);
void    kfree   (void *ptr);

size_t  km_total(void);   /* bytes administrados   */
size_t  km_used (void);   /* bytes actualmente en uso */


#endif