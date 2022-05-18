
C_SOURCES = $(shell find ./ -type f -name '*.c')
S_SOURCES = $(shell find ./ -type f -name '*.s')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o} ${S_SOURCES:.s=.o}

# https://wiki.osdev.org/ELF
build: $(OBJ)

%.o: %.c
	$(CC) -std=c17 -c $< -o $@

%.o: %.asm
	${NASM} $< -f elf64 -o $@

%.bin: %.s
	${NASM} $< -f elf64 -o $@

clean:

