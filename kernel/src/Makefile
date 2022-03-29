# $@ = target file
# $< = first dependency
# $^ = all dependencies

include ../../Makefile.conf

#CC = /usr/bin/clang
CC = ../../${COMPILER_PATH}${COMPILER_ARCH}gcc
CPP = ../../${COMPILER_PATH}${COMPILER_ARCH}g++
LD = ../../${COMPILER_PATH}${COMPILER_ARCH}ld
AS = ../../${COMPILER_PATH}${COMPILER_ARCH}as
OBJCOPY = ../../${COMPILER_PATH}${COMPILER_ARCH}objcopy
NASM = /usr/bin/nasm
GDB = ../../${COMPILER_PATH}${COMPILER_ARCH}gdb

SFN_SOURCES = $(shell find ./ -type f -name '*.sfn')
PSF_SOURCES = $(shell find ./ -type f -name '*.psf')
TARGZ_SOURCES = $(shell find ./ -type f -name '*.tar.gz')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
S_SOURCES = $(shell find ./ -type f -name '*.s')
SS_SOURCES = $(shell find ./ -type f -name '*.S')
C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
MAKE_DEPS_SOURCES = $(shell find ./ -type f -name '*.d')
HEADERS = $(sort $(dir $(wildcard ../include/*)))
OBJ = ${C_SOURCES:.c=.o} ${CPP_SOURCES:.cpp=.o} ${ASM_SOURCES:.asm=.o} ${S_SOURCES:.s=.o} ${SS_SOURCES:.S=.o} ${SFN_SOURCES:.sfn=.o} ${PSF_SOURCES:.psf=.o} ${TARGZ_SOURCES:.tar.gz=.o}
INCLUDE_DIR = ../include
KERNEL = kernel.fsys

CFLAGS = -pipe

INTERNALLDFLAGS := 							\
	-fno-pic -fno-pie 						\
	-Wl,-static,--no-dynamic-linker,-ztext 	\
	-nostdlib -nodefaultlibs -nolibc  		\
	-zmax-page-size=0x1000					\
	-Wl,-Map kernel.map

# Disable all warnings by adding "-w" in WARNCFLAG and if you want to treat the warnings as errors, add "-Werror"
WARNCFLAG = -Wall -Wextra -w

# Change march with minimum supported cpu // https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html
INTERNALCFLAGS :=													\
	-I${INCLUDE_DIR}												\
	-DKERNEL_VERSION='"$(KERNEL_VERSION)"'							\
	-fstack-protector-all											\
	-fno-pic -fno-pie -mno-80387									\
	-mno-80387 -mno-mmx -mno-3dnow -mno-red-zone -mno-sse -mno-sse2	\
	-march=nehalem													\
	-mcmodel=kernel -msoft-float -fno-builtin

ifeq ($(DEBUG), 1)
	INTERNALCFLAGS += -DDEBUG -ggdb -O0 -fdiagnostics-color=always -fsanitize=undefined
	INTERNALLDFLAGS += -ggdb -O0 -O2
	INTERNALCFLAGS += -DTRACING
	WARNCFLAG += -Wno-unused-function -Wno-maybe-uninitialized -Wno-builtin-declaration-mismatch -Wno-unknown-pragmas -Wno-unused-parameter -Wno-unused-variable
endif

ifeq ($(BOOTLOADER), 0)
	INTERNALLDFLAGS += -Tlinker.ld
	INTERNALCFLAGS += -DFENNIXBOOT
endif

ifeq ($(BOOTLOADER), 1)
	INTERNALLDFLAGS += -Tlinker-stivale.ld
	INTERNALCFLAGS += -DSTIVALE
endif

ifeq ($(BOOTLOADER), 2)
	INTERNALLDFLAGS += -Tlinker-stivale2.ld
	INTERNALCFLAGS += -DSTIVALE2
endif

ifeq ($(BOOTLOADER), 3)
	INTERNALLDFLAGS += -Tlinker-multiboot.ld
	INTERNALCFLAGS += -DMULTIBOOT
endif

ifeq ($(BOOTLOADER), 4)
	INTERNALLDFLAGS += -Tlinker-multiboot2.ld
	INTERNALCFLAGS += -DMULTIBOOT2
endif

default:
	$(error "No option specified! You may want to use: build DEBUG=1 BOOTLOADER=0")

build: $(KERNEL)

$(KERNEL): $(OBJ)
	$(CC) $(INTERNALLDFLAGS) $(OBJ) -o $@

%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) $(WARNCFLAG) $(INTERNALCFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp ${HEADERS}
	$(CPP) $(CFLAGS) $(WARNCFLAG) $(INTERNALCFLAGS) -std=c++20 -fexceptions -c $< -o $@ -fno-rtti

%.o: %.sfn
	$(OBJCOPY) -O elf64-x86-64 -I binary $< $@
	nm $@

%.o: %.psf
	$(OBJCOPY) -O elf64-x86-64 -I binary $< $@
	nm $@

%.o: %.tar.gz
	$(OBJCOPY) -O elf64-x86-64 -I binary $< $@
	nm $@

%.o: %.asm
	${NASM} $< -f elf64 -o $@

%.bin: %.asm
	${NASM} $< -f bin -o $@

%.bin: %.s
	${NASM} $< -f elf64 -o $@

%.o: %.s
	$(AS) -o $@ $<

%.o: %.S
	$(AS) -o $@ $<

clean:
	rm -f *.bin *.o *.elf *.fsys *.sym kernel.map initrd.tar.gz ${OBJ} ${MAKE_DEPS_SOURCES}