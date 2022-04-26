
C_SOURCES = $(shell find ./ -type f -name '*.c')
S_SOURCES = $(shell find ./ -type f -name '*.s')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o} ${S_SOURCES:.s=.o}


build: $(OBJ)
	mv $^ ../out/crt/

%.o: %.c
	$(CC) -std=c17 -c $< -o $@

%.o: %.asm
	${NASM} $< -f elf64 -o $@

%.bin: %.s
	${NASM} $< -f elf64 -o $@

clean:
	rm -f ../out/crt/*.o
