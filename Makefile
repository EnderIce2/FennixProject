
include Makefile.conf

QEMUFLAGS = -vga std \
			-usb \
			-usbdevice mouse \
			-smp $(shell nproc) \
			-net nic,model=rtl8139 \
			-net nic,model=pcnet \
			-serial file:serial.log \
			-net user \
			-drive id=disk,file=qemu-disk.img,if=none \
			-device ahci,id=ahci \
			-device ide-hd,drive=disk,bus=ahci.0 \
			-drive file=qemu-disk-ata.img,index=1,if=ide,format=raw \
			-device AC97 \
			-device sb16 \
			-device ES1370 \
			-device intel-hda -device hda-duplex \
			-soundhw pcspk \

QEMUHWACCELERATION = -machine q35 -M q35 -enable-kvm

# SYSTEM_MEM = $(shell grep 'MemTotal' /proc/meminfo | sed -e 's/MemTotal://' -e 's/ kB//')
# I could use $(shell echo ${SYSTEM_MEM}/1024/4 | bc) to specify a small amount (1/4) of memory for the qemu.
QEMUMEMORY = -m 4G

QEMU = ./${QEMU_PATH}

.PHONY: default tools clean

# First rule
default:
	$(info Hello! If you want to build the ISO & IMG file, please use the following command: make build)
	$(info If you want to quickly build and run in QEMU, please use the following command: make run)
	$(info If you want to clean, please use the following command: make clean)
	$(info Thanks.)

doxygen:
	mkdir -p doxygen-doc
	doxygen Doxyfile
	doxygen Doxyfile_headers

qemu_vdisk:
ifneq (,$(wildcard ./qemu-disk.img))
	$(info qemu-disk.img Already exists)
else
	dd if=/dev/zero of=qemu-disk.img bs=512 count=93750
	mformat -i qemu-disk.img -f 2880 ::
	@for f in $(shell ls resources/qemu_disk); do mcopy -i qemu-disk.img resources/qemu_disk/$${f} ::; done
	cp qemu-disk.img qemu-disk-ata.img
endif

# install necessary packages, compile cross-compiler etc..
tools:
	make --quiet -C tools all

# build the operating system
build_:
	tar cf initrd.tar.gz -C resources/initrd/ ./ --format=ustar
	make --quiet -C kernel build

build: quick_build build_image

rebuild: clean build

# quickly build the operating system (it won't create the ISO file and doxygen documentation)
quick_build:
	tar cf initrd.tar.gz -C resources/initrd/ ./ --format=ustar
	make --quiet -C kernel build

build_image:
	mkdir -p limine-bootloader
	cp kernel/src/kernel.fsys limine.cfg initrd.tar.gz \
		${LIMINE_FOLDER}/limine.sys \
		${LIMINE_FOLDER}/limine-cd.bin \
		${LIMINE_FOLDER}/limine-cd-efi.bin \
		limine-bootloader/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		limine-bootloader -o $(OSNAME).iso

vscode_debug: quick_build build_image
	rm -f serial.log
	${QEMU} -S -gdb tcp::1234 -d int -no-shutdown -no-reboot -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -m 4G ${QEMUFLAGS}

# build the os and run it
run: quick_build qemu_vdisk build_image
	rm -f serial.log
	${QEMU} -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -cpu host ${QEMUFLAGS} ${QEMUHWACCELERATION} ${QEMUMEMORY}

# clean
clean:
	rm -rf doxygen-doc limine-bootloader initrd.tar.gz *.iso
	make -C tools clean
	make -C kernel clean
