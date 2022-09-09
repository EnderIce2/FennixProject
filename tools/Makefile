WORKING_DIR = $(shell pwd)
CROSS_DIR="$(WORKING_DIR)/cross"
export PATH := $(CROSS_DIR):$(PATH)

QEMU_VERSION = qemu-7.1.0

all: do_initrd do_readethernet do_e do_limine clone_all do_binutils64 do_binutils32 do_binutilsarm64 do_gcc64 do_gcc32 do_gccarm64 do_qemu

clean:
	rm -f initrd.elf readethernet.elf e.elf

do_initrd:
	gcc -w initrd.c -o initrd.elf
	chmod +x initrd.elf

do_readethernet:
	gcc -w ReadEthernet.c -o readethernet.elf
	chmod +x readethernet.elf

do_e:
	g++ -w e.cpp -o e.elf
	chmod +x e.elf

do_limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1

clone_all:
	git clone https://sourceware.org/git/binutils-gdb.git binutils-gdb
	git clone https://gcc.gnu.org/git/gcc.git gcc
	wget https://download.qemu.org/${QEMU_VERSION}.tar.xz
	tar xvJf ${QEMU_VERSION}.tar.xz
	rm -f ${QEMU_VERSION}.tar.xz
	mv ${QEMU_VERSION} qemu
	mkdir -p cross
	mkdir -p qemu
	mkdir -p build-binutilsamd64
	mkdir -p build-binutilsi686
	mkdir -p build-binutilsarm64
	mkdir -p build-gccamd64
	mkdir -p build-gcci686
	mkdir -p build-gccarm64

do_qemu:
	cd qemu && \
	bash ./configure --target-list=x86_64-softmmu,i386-softmmu,aarch64-softmmu --enable-gtk && \
	make -j$(shell nproc)

do_binutils64:
	cd build-binutilsamd64 && \
	../binutils-gdb/configure --target=amd64-elf --prefix=$(CROSS_DIR) --with-sysroot --disable-werror && \
	make all -j$(shell nproc) && make install

do_binutils32:
	cd build-binutilsi686 && \
	../binutils-gdb/configure --target=i686-elf --prefix=$(CROSS_DIR) --with-sysroot --disable-werror && \
	make all -j$(shell nproc) && make install

do_binutilsarm64:
	cd build-binutilsarm64 && \
	../binutils-gdb/configure --target=aarch64-elf --prefix=$(CROSS_DIR) --with-sysroot --disable-werror && \
	make all -j$(shell nproc) && make install

do_gcc64:
	cd build-gccamd64 && \
	../gcc/configure --target=amd64-elf --prefix=$(CROSS_DIR) --enable-languages=c,c++ --without-headers && \
	make all-gcc -j$(shell nproc) && make install-gcc -j$(shell nproc)

do_gcc32:
	cd build-gcci686 && \
	../gcc/configure --target=i686-elf --prefix=$(CROSS_DIR) --enable-languages=c,c++ --without-headers && \
	make all-gcc -j$(shell nproc) && make install-gcc -j$(shell nproc)

do_gccarm64:
	cd build-gccarm64 && \
	../gcc/configure --target=aarch64-elf --prefix=$(CROSS_DIR) --enable-languages=c,c++ --without-headers && \
	make all-gcc -j$(shell nproc) && make install-gcc -j$(shell nproc)
