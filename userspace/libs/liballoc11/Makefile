include ../../../Makefile.conf

NAME=alloc11

OBJECT_NAME=lib$(NAME).o
FINAL_NAME=lib$(NAME).a

OUTPUT_DIR=../../../resources/initrd/system/libs/

CC = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
AR = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)ar

C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
HEADERS = $(sort $(dir $(wildcard ../../../include/*)))
INCLUDE_DIR = ../../../include

build: $(OBJECT_NAME)
	$(AR) -rcs $(OUTPUT_DIR)$(FINAL_NAME) $(OUTPUT_DIR)$(OBJECT_NAME)
	rm -f $(OUTPUT_DIR)$(OBJECT_NAME)

$(OBJECT_NAME): $(CPP_SOURCES)
	$(CC) -I$(INCLUDE_DIR) -nostdlib -fPIC -std=c++2a $(C_SOURCES) $(CPP_SOURCES) -o $(OUTPUT_DIR)$(OBJECT_NAME)

clean:
	rm -f $(OUTPUT_DIR)$(OBJECT_NAME) $(OUTPUT_DIR)$(FINAL_NAME)
