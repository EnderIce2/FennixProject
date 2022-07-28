include ../../../../Makefile.conf

NAME=wm
OUTPUT_DIR=../../../../resources/initrd/system/

CC = ../../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc

CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
HEADERS = $(sort $(dir $(wildcard ../../../../libc/include/*)))
INCLUDE_DIR = ../../../../libc/include

build: $(NAME)

$(NAME): $(CPP_SOURCES)
	$(CC) -I$(INCLUDE_DIR) --sysroot=../../../../resources/initrd/system -fPIC -std=c++2a $(CPP_SOURCES) -o $(OUTPUT_DIR)$(NAME)

clean:
	rm -f $(OUTPUT_DIR)$(NAME)
