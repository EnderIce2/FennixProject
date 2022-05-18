
WORKING_DIR = $(shell pwd)
CROSS_DIR="$(WORKING_DIR)/cross"
CROSS_TARGET=x86_64-elf
export PATH := $(CROSS_DIR):$(PATH)

VERSION = qemu-7.0.0

all: do_initrd do_limine do_binutils do_gcc do_qemu

clean:
	rm -f initrd.elf

do_initrd:
	gcc -w initrd.c -o initrd.elf

do_qemu:
	wget https://download.qemu.org/${VERSION}.tar.xz && \
	tar xvJf ${VERSION}.tar.xz && \
	rm -f ${VERSION}.tar.xz && \
	mv ${VERSION} qemu && \
	cd qemu && \
	bash ./configure --target-list=x86_64-softmmu && \
	make -j$(shell nproc)

do_limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1

do_binutils:
	git clone https://sourceware.org/git/binutils-gdb.git binutils-gdb && \
	mkdir -p build-binutils && \
	cd build-binutils && \
	../binutils-gdb/configure --target=$(CROSS_TARGET) --prefix=$(CROSS_DIR) --with-sysroot --disable-werror && \
	make -j$(shell nproc) && \
	make install

do_gcc:
	git clone https://gcc.gnu.org/git/gcc.git gcc && \
	mkdir -p build-gcc && \
	cd build-gcc && \
	../gcc/configure --target=$(CROSS_TARGET) --prefix=$(CROSS_DIR) --enable-languages=c,c++ --without-headers && \
	make all-gcc -j$(shell nproc) && \
	make install-gcc -j$(shell nproc)
