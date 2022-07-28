include ../../../../Makefile.conf

NAME=bfc
OUTPUT_DIR=../../../../resources/initrd/home/default/apps/

CC = ../../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc

C_SOURCES = $(shell find ./ -type f -name '*.c')
HEADERS = $(sort $(dir $(wildcard ../../../../libc/include/*)))
INCLUDE_DIR = ../../../../libc/include

build: $(NAME)

$(NAME): $(C_SOURCES)
	$(CC) -I$(INCLUDE_DIR) --sysroot=../../../../resources/initrd/system -fPIC -std=c17 $(C_SOURCES) -o $(OUTPUT_DIR)$(NAME)

clean:
	rm -f $(OUTPUT_DIR)$(NAME)
