# TODO: check if is a new version

define BUILD_SH
set -e
export PREFIX="$$PWD/cross"
export TARGET=x86_64-elf
export PATH="$$PREFIX/bin:$$PATH"
clear
mkdir -p build-binutils && cd build-binutils
../binutils-gdb/configure --target=$$TARGET --prefix="$$PREFIX" --with-sysroot --disable-werror
make -j$(shell nproc)
make install
cd ..
which -- $$TARGET-as || echo $$TARGET-as is not in the PATH
mkdir -p build-gcc && cd build-gcc
../gcc/configure --target=$$TARGET --prefix="$$PREFIX" --enable-languages=c,c++ --without-headers
make all-gcc -j$(shell nproc)
make all-target-libgcc -j$(shell nproc)
make install-gcc -j$(shell nproc)
make install-target-libgcc -j$(shell nproc)
cd ..

endef
export BUILD_SH

all: update_limine update_gcc update_qemu create_build

initrd:
	gcc -w initrd.c -o initrd.elf

clean:
	rm -f initrd.elf

create_build:
	$(shell rm -f $$PWD/build.sh)
	@echo "$$BUILD_SH" > build.sh
	chmod +x build.sh
	$(shell ./build.sh)

VERSION = qemu-6.2.0

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
