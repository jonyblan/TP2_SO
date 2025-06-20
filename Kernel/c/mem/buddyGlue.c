#include "buddyAllocator.h"
#include "memoryManager.h"     /* declara simple_* */
#include "memoryManagerSimple.h"

#ifdef USE_SIMPLE_MM         /* definido vía compilación */
#  define BACK_alloc   simple_alloc
#  define BACK_free    simple_free
#  define BACK_total   simple_total
#  define BACK_used    simple_used
#else                       /* por defecto: buddy */
#  define BACK_alloc   buddy_alloc
#  define BACK_free    buddy_free
#  define BACK_total   buddy_total
#  define BACK_used    buddy_used
#endif

void  *kmalloc(size_t s) { return BACK_alloc(s); }
void   kfree  (void *p)  { BACK_free(p);         }
size_t km_total(void)    { return BACK_total();  }
size_t km_used (void)    { return BACK_used();   }