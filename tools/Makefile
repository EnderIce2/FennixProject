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

# mkdir -p build-llvm && cd build-llvm
# cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS=lld -DCMAKE_INSTALL_PREFIX="$$PREFIX" ../llvm-project/llvm
# make install
# cd ..

all: update_limine update_gcc create_build

initrd:
	gcc -w initrd.c -o initrd.elf

clean:
	rm -f initrd.elf

create_build:
	$(shell rm -f $$PWD/build.sh)
	@echo "$$BUILD_SH" > build.sh
	chmod +x build.sh
	$(info Now open terminal in FennixProject/tools/ and run ./build.sh to build the crosscompiler.)

update_limine:
ifneq (,$(wildcard ./limine))
	$(info Don't forget to update the limine folder by removing it!)
else
	git clone https://github.com/limine-bootloader/limine.git --branch=latest-binary --depth=1
endif

update_gcc:
ifneq (,$(wildcard ./binutils-gdb))
	cd binutils-gdb && git pull git://sourceware.org/git/binutils-gdb.git && cd ..
else
	git clone git://sourceware.org/git/binutils-gdb.git binutils-gdb
endif
ifneq (,$(wildcard ./gcc))
	cd gcc && git pull git://gcc.gnu.org/git/gcc.git && cd ..
else
	git clone git://gcc.gnu.org/git/gcc.git gcc
endif

# update_llvm:
# ifneq (,$(wildcard ./llvm-project))
# 	cd gcc && git pull https://github.com/llvm/llvm-project && cd ..
# else
# 	git clone https://github.com/llvm/llvm-project llvm-project
# endif

# update_tomatoboot:
# ifneq (,$(wildcard ./tomatoboot))
# 	cd tomatoboot && make all
# else
# 	git clone https://github.com/TomatOrg/TomatBoot.git tomatoboot
# 	cd tomatoboot && make all
# endif