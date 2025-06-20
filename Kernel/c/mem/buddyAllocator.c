#include <stdint.h>
#include <stddef.h>
#include <lib.h>                
#include "buddyAllocator.h"

#define HEAP_START  0x1000000           /*  <- linker-script           */
#define HEAP_SIZE   (1 << 20)           /* 1 MiB                       */

#define MIN_ORDER   6                   /* 2^6  = 64 B  (bloque mín)   */
#define LEVELS      15                  /* 64 B … 1 MiB                */

typedef struct FreeBlock {
    struct FreeBlock *next;
} FreeBlock;

/* 15 listas de libres – una por orden                             */
static FreeBlock *free_lists[LEVELS] = {0};

/* Contabilidad para comando `mem`                                 */
static size_t used_bytes   = 0;
static size_t total_bytes  = HEAP_SIZE;

#define ALIGN_UP(sz)  (((sz)+((1U<<MIN_ORDER)-1)) & ~((1U<<MIN_ORDER)-1))

static inline int order_for_size(size_t size)
{
    size_t s = 1U << MIN_ORDER;
    int order = 0;
    while (s < size) { s <<= 1; order++; }
    return order;
}

static inline uintptr_t buddy_of(uintptr_t addr, int order)
{
    return addr ^ (1UL << (order + MIN_ORDER));
}

/* ---------- init ------------------------------------------------- */
static void buddy_init(void)
{
    for (int i = 0; i < LEVELS; i++) free_lists[i] = NULL;

    FreeBlock *first = (FreeBlock *)HEAP_START;
    first->next      = NULL;
    free_lists[LEVELS-1] = first;          /* bloque de 1 MiB           */
}

/* ---------- alloc ------------------------------------------------ */
void *buddy_alloc(size_t size)
{
    if (!free_lists[LEVELS-1]) buddy_init();     /* primera vez        */

    if (size == 0) return NULL;
    size = ALIGN_UP(size);
    int want = order_for_size(size);

    /* 1. buscar primer free-list con bloque */
    int i = want;
    while (i < LEVELS && !free_lists[i]) i++;
    if (i == LEVELS) return NULL;                 /* sin memoria        */

    /* 2. sacar bloque y hacer split hasta order deseado */
    FreeBlock *block = free_lists[i];
    free_lists[i]    = block->next;

    while (i > want) {
        i--;
        FreeBlock *buddy = (FreeBlock *)((uint8_t*)block
                              + (1UL << (i + MIN_ORDER)));
        buddy->next   = free_lists[i];
        free_lists[i] = buddy;
    }

    /* 3. guardar order en cabecera y devolver             */
    *((uint32_t*)block) = want;
    used_bytes += (1UL << (want + MIN_ORDER));
    return (void*)((uint8_t*)block + sizeof(uint32_t));
}

/* ---------- free ------------------------------------------------- */
void buddy_free(void *ptr)
{
    if (!ptr) return;

    uint8_t  *p    = (uint8_t*)ptr - sizeof(uint32_t);
    int       ord  = *((uint32_t*)p);
    uintptr_t addr = (uintptr_t)p;

    used_bytes -= (1UL << (ord + MIN_ORDER));

    /* intentar fusionar de abajo hacia arriba               */
    while (ord < LEVELS-1) {
        uintptr_t buddy_addr = buddy_of(addr, ord);

        /* buscar buddy en free-list de este orden           */
        FreeBlock **prev = &free_lists[ord];
        FreeBlock  *curr =  free_lists[ord];

        while (curr && (uintptr_t)curr != buddy_addr) {
            prev = &curr->next;
            curr =  curr->next;
        }
        if (!curr) break;                        /* buddy ocupado      */

        /* quitar buddy de lista y fusionar                  */
        *prev = curr->next;
        addr  = (addr < buddy_addr) ? addr : buddy_addr;
        ord++;
    }

    /* insertar bloque fusionado en su lista                 */
    FreeBlock *blk = (FreeBlock*)addr;
    blk->next      = free_lists[ord];
    free_lists[ord]= blk;
}

/* ---------- API de estadísticas --------------------------------- */
size_t buddy_total(void) { return total_bytes; }
size_t buddy_used (void) { return used_bytes;  }

void buddy_stats(char *buf, size_t n)
{
    int len = 0;
    len += snprintf(buf+len, n-len,
           "Buddy allocator – %zu B total, %zu B used (%zu B free)\n",
           total_bytes, used_bytes, total_bytes - used_bytes);

    for (int i = 0; i < LEVELS; i++) {
        int cnt = 0;
        for (FreeBlock *b = free_lists[i]; b; b = b->next) cnt++;
        len += snprintf(buf+len, n-len,
               "  %2d | size %6u B | blocks %-3d\n",
               i, 1U << (i+MIN_ORDER), cnt);
    }
}
