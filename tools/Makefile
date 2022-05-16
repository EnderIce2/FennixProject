all: update_limine update_gcc update_qemu create_binutils create_gcc

appveyor: update_limine
	wget https://dl.dropboxusercontent.com/s/6h3qsmw0ww7nsyd/cross.tar.xz?dl=0 -O cross.tar.xz
	tar -xvf cross.tar.xz

initrd:
	gcc -w initrd.c -o initrd.elf

clean:
	rm -f initrd.elf

WORKING_DIR = $(shell pwd)
CROSS_DIR="$(WORKING_DIR)/cross"
CROSS_TARGET=x86_64-elf
export PATH := $(CROSS_DIR):$(PATH)

create_binutils:
	mkdir -p build-binutils && \
	cd build-binutils && \
	../binutils-gdb/configure --target=$(CROSS_TARGET) --prefix=$(CROSS_DIR) --with-sysroot --disable-werror && \
	make -j$(shell nproc) && \
	make install

create_gcc:
	mkdir -p build-gcc && \
	cd build-gcc && \
	../gcc/configure --target=$(CROSS_TARGET) --prefix=$(CROSS_DIR) --enable-languages=c,c++ --without-headers && \
	make all-gcc -j$(shell nproc) && \
	make all-target-libgcc -j$(shell nproc) && \
	make install-gcc -j$(shell nproc) && \
	make install-target-libgcc -j$(shell nproc)

VERSION = qemu-7.0.0

update_qemu:
	wget https://download.qemu.org/${VERSION}.tar.xz && tar xvJf ${VERSION}.tar.xz && rm -f ${VERSION}.tar.xz && mv ${VERSION} qemu && cd qemu && bash ./configure --target-list=x86_64-softmmu && make -j$(shell nproc)

update_limine:
ifneq (,$(wildcard ./limine))
	$(info Don't forget to update the limine folder by removing it!)
else
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
endif

update_gcc:
ifneq (,$(wildcard ./binutils-gdb))
	cd binutils-gdb && git pull 	https://sourceware.org/git/binutils-gdb.git && cd ..
else
	git clone 	https://sourceware.org/git/binutils-gdb.git binutils-gdb
endif
ifneq (,$(wildcard ./gcc))
	cd gcc && git pull https://gcc.gnu.org/git/gcc.git && cd ..
else
	git clone https://gcc.gnu.org/git/gcc.git gcc
endif
