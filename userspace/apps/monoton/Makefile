include ../../../Makefile.conf

NAME=monoton
OUTPUT_DIR=../../../resources/initrd/system/

LIBS_DIR=../../../resources/initrd/system/libs/
LIBC_DIR=../../../libc/out
LIBC_crt=$(LIBC_DIR)/crt/crt

CC = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
LD = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)ld

C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
HEADERS = $(sort $(dir $(wildcard ../../../include/*)))
INCLUDE_DIR = ../../../include

build: $(NAME)

# for some reason static linking is not working. | -L$(LIBS_DIR) -lalloc11
# https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index

$(NAME): $(CPP_SOURCES)
	$(CC) -I$(INCLUDE_DIR) -nostdlib -fPIC -std=c++2a $(C_SOURCES) $(CPP_SOURCES) $(LIBC_crt)1.o -o $(OUTPUT_DIR)$(NAME)

clean:
	rm -f $(OUTPUT_DIR)$(NAME)
