# $@ = target file
# $< = first dependency
# $^ = all dependencies

include ../Makefile.conf

#CC = /usr/bin/clang
CC = ../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
CPP = ../$(COMPILER_PATH)/$(COMPILER_ARCH)g++
LD = ../$(COMPILER_PATH)/$(COMPILER_ARCH)ld
AS = ../$(COMPILER_PATH)/$(COMPILER_ARCH)as
OBJCOPY = ../$(COMPILER_PATH)/$(COMPILER_ARCH)objcopy
OBJDUMP = ../$(COMPILER_PATH)/$(COMPILER_ARCH)objdump
GDB = ../$(COMPILER_PATH)/$(COMPILER_ARCH)gdb
NASM = /usr/bin/nasm

GIT_COMMIT ?= unknown
GIT_COMMIT_SHORT ?= unknown

BMP_SOURCES = $(shell find ./ -type f -name '*.bmp')
PSF_SOURCES = $(shell find ./ -type f -name '*.psf')
ifeq ($(OSARCH), amd64)
ASM_SOURCES = $(shell find ./ -type f -name '*.asm' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
else ifeq ($(OSARCH), i686)
ASM_SOURCES = $(shell find ./ -type f -name '*.asm' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
else ifeq ($(OSARCH), aarch64)
ASM_SOURCES = $(shell find ./ -type f -name '*.asm' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
endif
MAKE_DEPS_SOURCES = $(shell find ./ -type f -name '*.d')
HEADERS = $(sort $(dir $(wildcard ./include/*)))
HEADERS2 = $(sort $(dir $(wildcard ../include/*)))
OBJ = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o) $(ASM_SOURCES:.asm=.o) $(S_SOURCES:.S=.o) $(PSF_SOURCES:.psf=.o) $(BMP_SOURCES:.bmp=.o)
INCLUDE_DIR = ./include
INCLUDE_DIR2 = ../include
KERNEL = kernel.fsys

LDFLAGS := 									\
	-fno-pic -fno-pie 						\
	-Wl,-static,--no-dynamic-linker,-ztext 	\
	-nostdlib -nodefaultlibs -nolibc  		\
	-zmax-page-size=0x1000					\
	-Wl,-Map kernel.map -shared

ifeq ($(OSARCH), amd64)
NASMFLAGS := -f elf64 -DAMD64=1
else ifeq ($(OSARCH), i686)
NASMFLAGS := -f elf32 -Di686=1
else ifeq ($(OSARCH), aarch64)
NASMFLAGS := -f elf32 -DARM64=1
endif

# Disable all warnings by adding "-w" in WARNCFLAG and if you want to treat the warnings as errors, add "-Werror"
WARNCFLAG = -Wall -Wextra -Wno-comment

# Change march with minimum supported cpu // https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html
CFLAGS :=										\
	-I$(INCLUDE_DIR) -I$(INCLUDE_DIR2)			\
	-DKERNEL_NAME='"$(OSNAME)"' 				\
	-DKERNEL_VERSION='"$(KERNEL_VERSION)"'		\
	-DGIT_COMMIT='"$(GIT_COMMIT)"'				\
	-DGIT_COMMIT_SHORT='"$(GIT_COMMIT_SHORT)"'

ifeq ($(OSARCH), amd64)
CFLAGS += -fno-pic -fno-pie -mno-80387 -mno-mmx -mno-3dnow	\
		  -mno-red-zone -mno-sse -mno-sse2					\
		  -march=nehalem -pipe								\
		  -mcmodel=kernel -msoft-float -fno-builtin

CFLAG_STACK_PROTECTOR := -fstack-protector-all
LDFLAGS += -Tlinker-amd64.ld
else ifeq ($(OSARCH), i686)
CFLAGS += -fno-pic -fno-pie -mno-80387 -mno-mmx -mno-3dnow	\
		  -mno-red-zone -mno-sse -mno-sse2					\
		  -march=nehalem -pipe -msoft-float -fno-builtin

CFLAG_STACK_PROTECTOR := -fstack-protector-all
LDFLAGS += -Tlinker-i686.ld -fPIC
else ifeq ($(OSARCH), aarch64)
CFLAGS += -pipe -fno-builtin -fPIC

CFLAG_STACK_PROTECTOR := -fno-stack-protector
LDFLAGS += -Tlinker-aarch64.ld
endif

# -fsanitize=undefined is causing tasking issues
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -DTRACING -ggdb -O0 -fdiagnostics-color=always
	LDFLAGS += -ggdb -O0 -O2 -g
	NASMFLAGS += -F dwarf -g
	WARNCFLAG += -Wno-unused-function -Wno-maybe-uninitialized -Wno-builtin-declaration-mismatch -Wno-unknown-pragmas -Wno-unused-parameter -Wno-unused-variable
endif

ifeq ($(UNIT_TESTS), 1)
	CFLAGS += -DUNIT_TESTS
endif

default:
	$(error "No option specified!")

build: $(KERNEL)
	$(OBJDUMP) -d kernel.fsys > kernel_full.map

$(KERNEL): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c $(HEADERS) $(HEADERS2)
	$(CC) $(CFLAGS) $(CFLAG_STACK_PROTECTOR) $(WARNCFLAG) -std=c17 -c $< -o $@

# https://gcc.gnu.org/projects/cxx-status.html
%.o: %.cpp $(HEADERS) $(HEADERS2)
	$(CPP) $(CFLAGS) $(CFLAG_STACK_PROTECTOR) $(WARNCFLAG) -std=c++20 -fexceptions -c $< -o $@ -fno-rtti

%.o: %.asm
	$(NASM) $< $(NASMFLAGS) -o $@

%.o: %.S
ifeq ($(OSARCH), amd64)
	$(AS) --defsym __amd64__=1 -o $@ $<
else ifeq ($(OSARCH), i686)
	$(AS) --defsym __i386__=1 -o $@ $<
else ifeq ($(OSARCH), aarch64)
	$(AS) --defsym __aarch64__=1 -o $@ $<
endif

%.o: %.psf
ifeq ($(OSARCH), amd64)
	$(OBJCOPY) -O elf64-x86-64 -I binary $< $@
else ifeq ($(OSARCH), i686)
	$(OBJCOPY) -O elf32-i386 -I binary $< $@
else ifeq ($(OSARCH), aarch64)
	$(OBJCOPY) -O elf64-littleaarch64 -I binary $< $@
endif
	nm $@

%.o: %.bmp
ifeq ($(OSARCH), amd64)
	$(OBJCOPY) -O elf64-x86-64 -I binary $< $@
else ifeq ($(OSARCH), i686)
	$(OBJCOPY) -O elf32-i386 -I binary $< $@
else ifeq ($(OSARCH), aarch64)
	$(OBJCOPY) -O elf64-littleaarch64 -I binary $< $@
endif
	nm $@

clean:
	rm -f *.bin *.o *.elf *.fsys *.sym kernel.map kernel_full.map initrd.tar.gz $(OBJ) $(MAKE_DEPS_SOURCES)
