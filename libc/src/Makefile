include ../../Makefile.conf

NAME=c

OBJECT_NAME=lib$(NAME).a

OUTPUT_DIR=../../resources/initrd/system/lib/

CC = ../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
AR = ../../$(COMPILER_PATH)/$(COMPILER_ARCH)ar

C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
OBJ = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)
HEADERS = $(sort $(dir $(wildcard ../../include/*)))
INCLUDE_DIR = ../../include
HEADERS2 = $(sort $(dir $(wildcard ../include/*)))
INCLUDE2_DIR = ../include

LDFLAGS :=
CFLAGS := -fPIC -I$(INCLUDE_DIR) -I$(INCLUDE2_DIR)

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(AR) rcs  $(OUTPUT_DIR)$@ $(OBJ)
%.o: %.c $(HEADERS) $(HEADERS2)
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp $(HEADERS) $(HEADERS2)
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

clean:
	rm -f $(OUTPUT_DIR)$(OBJECT_NAME) $(OBJ)
