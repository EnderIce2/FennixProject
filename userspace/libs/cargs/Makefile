include ../../../Makefile.conf

NAME=cargs

OBJECT_NAME=lib$(NAME).a

OUTPUT_DIR=../../../resources/initrd/system/lib/

CC = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
AR = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)ar

C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
OBJ = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)
HEADERS = $(sort $(dir $(wildcard ../../../libc/include/*)))
INCLUDE_DIR = ../../../libc/include

LDFLAGS :=
CFLAGS := -I$(INCLUDE_DIR) -fPIC --sysroot=../../../resources/initrd/system/

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(AR) rcs $(OUTPUT_DIR)$@ $(OBJ)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

clean:
	rm -f $(OUTPUT_DIR)$(OBJECT_NAME) $(OBJ)
