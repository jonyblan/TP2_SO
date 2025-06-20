#include "memoryManager.h"          /* sólo para los tipos comunes   */
#include "memoryManagerSimple.h"    /* declara simple_*              */
#include <stdint.h>
#include <stddef.h>

/* ——— Wrappers opcionales ——— */

void *malloc(size_t n)  { return simple_alloc(n); }
void  free  (void *ptr) { simple_free(ptr); }

/* ------------ Parámetros del heap ------------------------ */
#define HEAP_START 0x1000000UL        /* 16 MiB (linker script)      */
#define HEAP_SIZE  0x00100000UL       /* 1 MiB administrados        */
#define ALIGN      64UL               /* alineación mínima          */

typedef struct Block {
    struct Block *next;
    size_t        size;
    uint8_t       free;
} Block;

static Block *free_list   = (Block *)HEAP_START;
static const  size_t heap_total = HEAP_SIZE;
static        size_t heap_used  = 0;

/* 1ª invocación -------------------------------------------------- */
static void init_heap(void)
{
    free_list->next = NULL;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
}

static inline size_t align_up(size_t s)
{
    return (s + ALIGN - 1) & ~(ALIGN - 1);
}

/* ---------- simple_alloc --------------------------------------- */
void *simple_alloc(size_t size)
{
    if (free_list->size == 0)          /* heap aún no inicializado   */
        init_heap();

    size = align_up(size);

    for (Block *b = free_list; b; b = b->next) {
        if (b->free && b->size >= size) {

            /* split si sobra espacio para otro bloque + header     */
            if (b->size >= size + sizeof(Block) + ALIGN) {
                Block *split = (Block *)((uint8_t *)b +
                                         sizeof(Block) + size);
                split->size = b->size - size - sizeof(Block);
                split->next = b->next;
                split->free = 1;

                b->next  = split;
                b->size  = size;
            }

            b->free = 0;
            heap_used += b->size;
            return b + 1;             /* user pointer justo después
                                         del header Block            */
        }
    }
    return NULL;                      /* sin hueco disponible        */
}

/* ---------- simple_free ---------------------------------------- */
void simple_free(void *ptr)
{
    if (!ptr) return;
    Block *b = (Block *)ptr - 1;
    b->free  = 1;
    heap_used -= b->size;

    /* fusionar hacia adelante si el siguiente bloque también está
       libre, para reducir fragmentación                            */
    if (b->next && b->next->free) {
        b->size += sizeof(Block) + b->next->size;
        b->next  = b->next->next;
    }
}

/* ---------- Estadísticas --------------------------------------- */
size_t simple_total(void) { return heap_total; }
size_t simple_used (void) { return heap_used;  }