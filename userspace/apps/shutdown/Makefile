include ../../../Makefile.conf

NAME=shutdown
OUTPUT_DIR=../../../resources/initrd/system/

CC = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
LD = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)ld

C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
OBJ = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)
HEADERS = $(sort $(dir $(wildcard ../../../libc/include/*)))
INCLUDE_DIR = ../../../libc/include

LDFLAGS := --sysroot=../../../resources/initrd/system/
CFLAGS := -fPIC -I$(INCLUDE_DIR)

build: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(OUTPUT_DIR)$@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

clean:
	rm -f $(OUTPUT_DIR)$(NAME) $(OBJ)
