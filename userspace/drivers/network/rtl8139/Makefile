include ../../../../Makefile.conf

NAME=Rtl8139.drv
OUTPUT_DIR=../../../../resources/initrd/system/drivers/

CC = ../../../../$(COMPILER_PATH)/$(COMPILER_ARCH)g++

CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
HEADERS = $(sort $(dir $(wildcard ../../../../include/*)))
INCLUDE_DIR = ../../../../include

build: $(NAME)

$(NAME): $(CPP_SOURCES)
	$(CC) -I$(INCLUDE_DIR) -Tlink.ld -nostdlib -fPIC -std=c++2a $(CPP_SOURCES) -o $(OUTPUT_DIR)$(NAME)

clean:
	rm -f $(OUTPUT_DIR)$(NAME)
