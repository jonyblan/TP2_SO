include Makefile.inc

# ────────────────────────────────────────────────────────────────
# 1) Backend de memoria
#    make                → simple
#    make MEMORY=buddy   → buddy-system
# ────────────────────────────────────────────────────────────────
MEMORY ?= simple            # valores válidos:  simple | buddy

ifeq ($(MEMORY),buddy)
    MM_SRC := c/mm/buddyAllocator.c \
              c/mm/buddyGlue.c \
			  c/mm/mm_wrappers.c  	
else
    MM_SRC := c/mm/memoryManager_simple.c
endif


# ────────────────────────────────────────────────────────────────
# 2) Fuentes
# ────────────────────────────────────────────────────────────────
SRC_TOP        := $(wildcard *.c)
SRC_ASM        := $(wildcard asm/*.asm)
SRC_C_COMMON   := $(wildcard c/*.c) \
                  $(wildcard c/Process/*.c) \
                  $(wildcard c/drivers/*.c) \
                  $(wildcard c/interruptions/*.c)

SRC_C          := $(filter-out $(MM_SRC),$(SRC_C_COMMON)) $(MM_SRC)

# ────────────────────────────────────────────────────────────────
# 3) Objetos
# ────────────────────────────────────────────────────────────────
OBJ_ASM   := $(SRC_ASM:.asm=.o)
OBJ_C     := $(SRC_C:.c=.o)
OBJ_ALL   := $(OBJ_ASM) $(OBJ_C)

LOADEROBJ := loader.o            # se genera aparte
KERNEL    := kernel.bin

# ────────────────────────────────────────────────────────────────
# 4) Targets
# ────────────────────────────────────────────────────────────────
all: $(KERNEL)

$(KERNEL): $(LOADEROBJ) $(OBJ_ALL)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $^

# reglas genéricas
%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJ): loader.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# ────────────────────────────────────────────────────────────────
# 5) Limpieza
# ────────────────────────────────────────────────────────────────
clean:
	rm -f $(OBJ_ALL) $(LOADEROBJ) $(KERNEL) kernel.elf
	rm -f asm/*.o c/*.o c/Process/*.o c/drivers/*.o \
	      c/interruptions/*.o c/mm/*.o

.PHONY: all clean