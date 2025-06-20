include Makefile.inc           # define GCC, LD, ASM, {GCC,ASM,LD}FLAGS

###############################################################################
#  Selección de backend de memoria                                            #
#  ▸ make                → usa Buddy (❙ DEFAULT)                              #
#  ▸ make BACKEND=simple  → usa heap simple                                   #
###############################################################################
ifeq ($(BACKEND),simple)
    GCCFLAGS += -DUSE_SIMPLE_MM          # activa simple_* en el glue
endif

# Desactiva intrínsecos de Clang/GCC que chocan con tus propias funciones
GCCFLAGS += -fno-builtin-memset -fno-builtin-memcpy \
            -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc

###############################################################################
#  Fuentes – rutas tal cual tu repositorio                                     #
###############################################################################
KERNEL      := kernel.bin

# .c en el raíz del kernel  (incluye buddyAllocator.c, memoryManagerGlue.c, etc.)
SOURCES      := $(wildcard *.c)

# Subcarpetas
SOURCES_ASM  := $(wildcard asm/*.asm)
SOURCES_C    := $(filter-out c/memoryManager.c, $(wildcard c/*.c))        # <- excluye el viejo backend
SOURCES_C   += $(wildcard c/Process/*.c)
SOURCES_DRIVERS        := $(wildcard c/drivers/*.c)
SOURCES_INTERRUPTIONS  := $(wildcard c/interruptions/*.c)

# Objetos
OBJECTS                := $(SOURCES:.c=.o)
OBJECTS_ASM            := $(SOURCES_ASM:.asm=.o)
OBJECTS_C              := $(SOURCES_C:.c=.o)
OBJECTS_DRIVERS        := $(SOURCES_DRIVERS:.c=.o)
OBJECTS_INTERRUPTIONS  := $(SOURCES_INTERRUPTIONS:.c=.o)

# Loader
LOADERSRC     := loader.asm
LOADEROBJECT  := $(LOADERSRC:.asm=.o)

STATICLIBS    :=               # añade .a si las necesitas

###############################################################################
#  Regla principal                                                             #
###############################################################################
all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_C) \
           $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ \
	       $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) \
	       $(OBJECTS_C) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(STATICLIBS)
	# ELF con símbolos para GDB
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf \
	       $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) \
	       $(OBJECTS_C) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(STATICLIBS)

###############################################################################
#  Reglas de compilación                                                       #
###############################################################################
%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT): $(LOADERSRC)
	$(ASM) $(ASMFLAGS) $< -o $@

###############################################################################
#  Limpieza                                                                    #
###############################################################################
clean:
	rm -rf asm/*.o *.o *.bin \
           c/*.o c/drivers/*.o c/interruptions/*.o

.PHONY: all clean