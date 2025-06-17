#include "memoryManager.h"
#include "buddyAllocator.h"

void  *kmalloc(size_t s) { return buddy_alloc(s); }
void   kfree (void *p)   { buddy_free (p);        }
size_t km_total(void)    { return buddy_total();  }
size_t km_used (void)    { return buddy_used ();  }