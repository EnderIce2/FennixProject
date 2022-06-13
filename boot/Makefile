
GNUEFI_RELEASE_VERSION=3.0.14

NASM = /usr/bin/nasm

gnuefi:
	wget https://deac-ams.dl.sourceforge.net/project/gnu-efi/gnu-efi-$(GNUEFI_RELEASE_VERSION).tar.bz2
	tar -xf gnu-efi-$(GNUEFI_RELEASE_VERSION).tar.bz2 -C UEFI
	rm gnu-efi-$(GNUEFI_RELEASE_VERSION).tar.bz2
	mv UEFI/gnu-efi-$(GNUEFI_RELEASE_VERSION) UEFI/gnu-efi
	cp -a ./UEFI/gnu-efi/inc/. ./UEFI/include
	make -C UEFI/gnu-efi

build:
	$(info Compiling UEFI bootloader...)
	make -C BIOS build
	make -C UEFI build

clean:
	make -C UEFI clean
	make -C BIOS clean
