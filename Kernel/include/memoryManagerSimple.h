#ifndef SIMPLE_MM_H
#define SIMPLE_MM_H
#include <stddef.h>

void  *simple_alloc(size_t size);
void   simple_free (void *ptr);
size_t simple_total(void);
size_t simple_used (void);

#endif