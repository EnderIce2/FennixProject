
include Makefile.conf

# Available display devices
# None:       -vga none
# Standard:   -device VGA
# Bochs:      -device bochs-display
# VirtIO VGA: -device virtio-vga
# VirtIO GPU: -device virtio-gpu-pci
# QXL VGA:    -device qxl-vga
# QXL:        -device qxl
# Cirrus VGA: -device cirrus-vga
# ATI VGA:    -device ati-vga
# RAMFB:      -device ramfb

QEMUFLAGS = -device bochs-display -M q35 \
			-usb -no-reboot \
			-usbdevice mouse \
			-smp $(shell nproc) \
			-net nic,model=rtl8139 \
			-net nic,model=pcnet \
			-serial file:serial.log \
			-net user \
			-drive id=disk,file=qemu-disk.img,if=none \
			-device ahci,id=ahci \
			-device ide-hd,drive=disk,bus=ahci.0 \
			-device AC97 \
			-device sb16 \
			-device ES1370 \
			-device intel-hda -device hda-duplex \
			-soundhw pcspk \

QEMUHWACCELERATION = -machine q35 -enable-kvm

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
	dd if=/dev/zero of=qemu-disk.img bs=1024K count=4000
endif

# download fonts for the kernel
fonts:
	wget https://www.zap.org.au/projects/console-fonts-zap/src/zap-ext-light20.psf -P kernel/files
	wget https://www.zap.org.au/projects/console-fonts-zap/src/zap-ext-light24.psf -P kernel/files
	wget https://www.zap.org.au/projects/console-fonts-zap/src/zap-light16.psf -P kernel/files
	wget https://raw.githubusercontent.com/powerline/fonts/master/Terminus/PSF/ter-powerline-v12n.psf.gz -P kernel/files
	gzip -d kernel/files/ter-powerline-v12n.psf.gz

# install necessary packages, compile cross-compiler etc..
tools: fonts
	make --quiet -C tools all

build: build_kernel build_libc build_userspace build_image

rebuild: clean build

# quickly build the operating system (it won't create the ISO file and doxygen documentation)
build_kernel:
	make -j$(shell nproc) --quiet -C kernel build

build_userspace:
	make --quiet -C userspace build

build_libc:
	make --quiet -C libc build

build_image:
	mkdir -p limine-bootloader
	tar cf initrd.tar.gz -C resources/initrd/ ./ --format=ustar
	cp kernel/kernel.fsys limine.cfg initrd.tar.gz startup.nsh \
		${LIMINE_FOLDER}/limine.sys \
		${LIMINE_FOLDER}/limine-cd.bin \
		${LIMINE_FOLDER}/limine-cd-efi.bin \
		limine-bootloader/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		limine-bootloader -o $(OSNAME).iso

vscode_debug: build_userspace build_kernel build_image
	rm -f serial.log
	${QEMU} -S -gdb tcp::1234 -d int -no-shutdown -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -m 4G ${QEMUFLAGS}

qemu: qemu_vdisk
	rm -f serial.log
	${QEMU} -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -cpu host ${QEMUFLAGS} ${QEMUHWACCELERATION} ${QEMUMEMORY}

# build the os and run it
run: build qemu_vdisk qemu

# clean
clean:
	rm -rf doxygen-doc limine-bootloader initrd.tar.gz *.iso
	make --quiet -C tools clean
	make --quiet -C kernel clean
	make --quiet -C userspace clean
	make --quiet -C libc clean