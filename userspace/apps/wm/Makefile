include ../../../Makefile.conf

NAME=wm
OUTPUT_DIR=../../../resources/initrd/system/

LIBC_DIR=../../../libc/out
LIBC_crt=$(LIBC_DIR)/crt/crt

CC = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc

CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
HEADERS = $(sort $(dir $(wildcard ../../../include/*)))
INCLUDE_DIR = ../../../include

build: $(NAME)

$(NAME): $(CPP_SOURCES)
	$(CC) -I$(INCLUDE_DIR) -nostdlib -fPIC -std=c++2a $(CPP_SOURCES) $(LIBC_crt)1.o -o $(OUTPUT_DIR)$(NAME)

clean:
	rm -f $(OUTPUT_DIR)$(NAME)
