#include <stddef.h>
#include "memoryManager.h"   /*  kmalloc / kfree  */

/*  Interfaz compatible con la libc ────────────── */
void *malloc(size_t n)  { return kmalloc(n); }
void  free (void *ptr)  { kfree(ptr);   }