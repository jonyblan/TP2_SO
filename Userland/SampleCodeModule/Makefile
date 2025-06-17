include ../Makefile.inc            # GCC, ASM, LD, GCCFLAGS, ASMFLAGS, LDFLAGS …

# ------------- nombre del módulo que exporta este sub-directorio -------------
MODULE := 0000-sampleCodeModule.bin     # <-- se verá luego en ../

# ------------- descubrimiento automático de fuentes --------------------------
#
#  - *.c / *.asm en este dir          (ej: _loader.c)
#  - c/*.c   : todos tus comandos (shell, wc, philosopher.c, …)
#  - asm/*.asm: wrappers de syscalls, etc.
#
SRC_ROOT_C   := $(wildcard *.c)
SRC_ROOT_ASM := $(wildcard *.asm)

SRC_C_DIR    := $(wildcard c/*.c)
SRC_ASM_DIR  := $(wildcard asm/*.asm)

# ------------- objetos intermedios ------------------------------------------
OBJ_ROOT_C   := $(SRC_ROOT_C:.c=.o)
OBJ_ROOT_ASM := $(SRC_ROOT_ASM:.asm=.o)

#   c/foo.c   -> c/foo.o   |   asm/bar.asm -> asm/bar.o
OBJ_C_DIR    := $(patsubst c/%.c,c/%.o,$(SRC_C_DIR))
OBJ_ASM_DIR  := $(patsubst asm/%.asm,asm/%.o,$(SRC_ASM_DIR))

OBJECTS      := $(OBJ_ROOT_C) $(OBJ_ROOT_ASM) $(OBJ_C_DIR) $(OBJ_ASM_DIR)

# ------------- reglas principales -------------------------------------------
all: $(MODULE)

# Link final
$(MODULE): $(OBJECTS)
	@echo "[LD]  $@"
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$@ $^

# ------------ reglas genéricas de compilación (root, c/, asm/) --------------
# root level .c
%.o: %.c
	@echo "[CC]  $<"
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

# root level .asm
%.o: %.asm
	@echo "[AS]  $<"
	$(ASM) $(ASMFLAGS) $< -o $@

# c/ folder .c
c/%.o: c/%.c
	@echo "[CC]  $<"
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

# asm/ folder .asm
asm/%.o: asm/%.asm
	@echo "[AS]  $<"
	$(ASM) $(ASMFLAGS) $< -o $@

# ------------- utilidades ----------------------------------------------------
clean:
	rm -f $(OBJECTS)

.PHONY: all clean