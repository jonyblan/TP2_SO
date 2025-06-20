###############################################################################
# Herramientas y flags genéricos (se definen en Makefile.inc)                 #
###############################################################################
include Makefile.inc          # define GCC, LD, ASM y {GCC,ASM,LD}FLAGS

# Elige backend: buddy (default) ───────────  make           → buddy
#                                            make BACKEND=simple  → heap simple
BACKEND ?= buddy

###############################################################################
# Selección de backend de memoria                                             #
###############################################################################
ifeq ($(BACKEND),simple)
    GCCFLAGS += -DUSE_SIMPLE_MM
    MM_SRC    := c/memoryManager.c
else                               # buddy (default)
    GCCFLAGS += -DUSE_BUDDY_MM
    MM_SRC    := c/buddyAllocator.c
endif

# Desactivar intrínsecos conflictivos de GCC/Clang
GCCFLAGS += -fno-builtin-memset -fno-builtin-memcpy \
            -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc

###############################################################################
# Fuentes                                                                      #
###############################################################################
KERNEL          := kernel.bin

# .c en el raíz del kernel
SOURCES_ROOT     := $(wildcard *.c)

# Subcarpetas
SOURCES_ASM      := $(wildcard asm/*.asm)
SOURCES_C_COMMON := $(filter-out c/memoryManager.c c/buddyAllocator.c, \
                     $(wildcard c/*.c))
SOURCES_C_COMMON += $(wildcard c/Process/*.c)
SOURCES_DRIVERS  := $(wildcard c/drivers/*.c)
SOURCES_INTR     := $(wildcard c/interruptions/*.c)

# Backend elegido
SOURCES_C        := $(SOURCES_C_COMMON) $(MM_SRC)

# Objetos
OBJECTS_ROOT           := $(SOURCES_ROOT:.c=.o)
OBJECTS_ASM            := $(SOURCES_ASM:.asm=.o)
OBJECTS_C              := $(SOURCES_C:.c=.o)
OBJECTS_DRIVERS        := $(SOURCES_DRIVERS:.c=.o)
OBJECTS_INTR           := $(SOURCES_INTR:.c=.o)

# Loader (Pure64/bootsector)
LOADERSRC     := loader.asm
LOADEROBJECT  := $(LOADERSRC:.asm=.o)

STATICLIBS    :=                               # añade .a si las usas

###############################################################################
# Regla principal                                                              #
###############################################################################
all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS_ROOT) $(OBJECTS_ASM) $(OBJECTS_C) \
           $(OBJECTS_DRIVERS) $(OBJECTS_INTR) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ \
	       $(LOADEROBJECT) $(OBJECTS_ROOT) $(OBJECTS_ASM) \
	       $(OBJECTS_C) $(OBJECTS_DRIVERS) $(OBJECTS_INTR) $(STATICLIBS)
	# ELF con símbolos para GDB
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf \
	       $(LOADEROBJECT) $(OBJECTS_ROOT) $(OBJECTS_ASM) \
	       $(OBJECTS_C) $(OBJECTS_DRIVERS) $(OBJECTS_INTR) $(STATICLIBS)

###############################################################################
# Reglas de compilación                                                        #
###############################################################################
%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT): $(LOADERSRC)
	$(ASM) $(ASMFLAGS) $< -o $@

###############################################################################
# Targets de conveniencia                                                      #
###############################################################################
simple:      ## Compila con heap simple
	$(MAKE) clean all BACKEND=simple

buddy:       ## Compila con buddy (default)
	$(MAKE) clean all BACKEND=buddy

###############################################################################
# Limpieza                                                                     #
###############################################################################
clean:
	$(RM) -rf asm/*.o *.o *.bin \
           c/**/*.o

.PHONY: all clean simple buddy