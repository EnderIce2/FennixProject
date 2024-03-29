
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

# For tap0
# -netdev tap,id=usernet0,ifname=tap0,script=no,downscript=no
QEMUFLAGS := -display gtk
ifeq ($(OSARCH), amd64)
QEMUFLAGS += -device bochs-display -M q35 \
			 -usb \
			 -usbdevice mouse \
			 -smp $(shell nproc) \
			 -net user \
			 -netdev user,id=usernet0 \
			 -device e1000,netdev=usernet0,mac=00:69:96:00:42:00 \
			 -object filter-dump,id=usernet0,netdev=usernet0,file=network.log,maxlen=1024 \
			 -serial file:serial.log \
			 -device ahci,id=ahci \
			 -drive id=bootdsk,file=$(OSNAME).iso,format=raw,if=none \
			 -device ide-hd,drive=bootdsk,bus=ahci.0 \
			 -drive id=disk,file=qemu-disk.img,format=raw,if=none \
			 -device ide-hd,drive=disk,bus=ahci.1 \
			 -audiodev pa,id=audio0 \
			 -machine pcspk-audiodev=audio0 \
			 -device AC97,audiodev=audio0

else ifeq ($(OSARCH), i686)
QEMUFLAGS += -M q35 \
			 -usb \
			 -usbdevice mouse \
			 -smp $(shell nproc) \
			 -net user \
			 -netdev user,id=usernet0 \
			 -device e1000,netdev=usernet0,mac=00:69:96:00:42:00 \
			 -object filter-dump,id=usernet0,netdev=usernet0,file=network.log,maxlen=1024 \
			 -serial file:serial.log \
			 -hda $(OSNAME).iso \
			 -audiodev pa,id=audio0 \
			 -machine pcspk-audiodev=audio0 \
			 -device AC97,audiodev=audio0
endif

QEMUHWACCELERATION = -machine q35 -enable-kvm

# SYSTEM_MEM = $(shell grep 'MemTotal' /proc/meminfo | sed -e 's/MemTotal://' -e 's/ kB//')
# I could use $(shell echo $(SYSTEM_MEM)/1024/4 | bc) to specify a small amount (1/4) of memory for the qemu.
QEMUMEMORY = -m 4G

QEMU = ./$(QEMU_PATH)$(QEMU_ARCH)

.PHONY: default tools clean

# First rule
default:
	$(error Please specify a target)

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

# Download fonts for the kernel.
fonts:
	rm -f ./kernel/files/zap-ext-light20.psf ./kernel/files/zap-ext-light24.psf ./kernel/files/zap-light16.psf ./kernel/files/ter-powerline-v12n.psf
	wget https://www.zap.org.au/projects/console-fonts-zap/src/zap-ext-light20.psf -P kernel/files
	wget https://www.zap.org.au/projects/console-fonts-zap/src/zap-ext-light24.psf -P kernel/files
	wget https://www.zap.org.au/projects/console-fonts-zap/src/zap-light16.psf -P kernel/files
	wget https://raw.githubusercontent.com/powerline/fonts/master/Terminus/PSF/ter-powerline-v12n.psf.gz -P kernel/files
	gzip -d kernel/files/ter-powerline-v12n.psf.gz

# Install necessary packages, build cross-compiler etc...
tools: fonts
	make --quiet -C tools all
	make --quiet -C boot gnuefi

tools_workflow0: fonts
	make --quiet -C tools do_initrd
	make --quiet -C tools do_readethernet
	make --quiet -C tools do_e

tools_workflow1:
	make --quiet -C tools do_limine

tools_workflow2:
	make --quiet -C tools clone_all

tools_workflow3:
	make --quiet -C tools do_binutils64

tools_workflow4:
	make --quiet -C tools do_binutils32

tools_workflow5:
	make --quiet -C tools do_binutilsarm64

tools_workflow6:
	make --quiet -C tools do_gcc64

tools_workflow7:
	make --quiet -C tools do_gcc32

tools_workflow8:
	make --quiet -C tools do_gccarm64

tools_workflow9:
	make --quiet -C boot gnuefi

build: build_bootloader build_kernel build_libc build_userspace build_image

rebuild: clean build

# Quickly build the operating system (it won't create the ISO file and doxygen documentation)
build_bootloader:
ifeq ($(BOOTLOADER), lynx)
	make --quiet -C boot build
endif

build_kernel:
	make -j$(shell nproc) --quiet -C kernel build GIT_COMMIT=$(shell git rev-parse HEAD) GIT_COMMIT_SHORT=$(shell git rev-parse --short HEAD)

build_userspace:
ifeq ($(OSARCH), amd64)
	make --quiet -C userspace build
endif

build_libc:
ifeq ($(OSARCH), amd64)
	make --quiet -C libc build
endif

build_image:
	mkdir -p iso_tmp_data
	tar cf initrd.tar.gz -C resources/initrd/ ./ --format=ustar
	cp kernel/kernel.fsys initrd.tar.gz startup.nsh \
		iso_tmp_data/
ifeq ($(BOOTLOADER), lynx)
	cp lynx.cfg boot/BIOS/loader.bin boot/UEFI/efi-loader.bin iso_tmp_data/
	xorriso -as mkisofs -b loader.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot efi-loader.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_tmp_data -o $(OSNAME).iso
endif
ifeq ($(BOOTLOADER), other)
ifeq ($(OSARCH), amd64)
	cp limine.cfg $(LIMINE_FOLDER)/limine.sys $(LIMINE_FOLDER)/limine-cd.bin $(LIMINE_FOLDER)/limine-cd-efi.bin iso_tmp_data/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_tmp_data -o $(OSNAME).iso
endif
ifeq ($(OSARCH), i686)
	mkdir -p iso_tmp_data/boot
	mkdir -p iso_tmp_data/boot/grub
	cp grub.cfg iso_tmp_data/boot/grub/
	grub-mkrescue -o $(OSNAME).iso iso_tmp_data
endif
endif

QEMU_UEFI_BIOS :=
ifeq ($(OSARCH), amd64)
QEMU_UEFI_BIOS += -bios /usr/share/qemu/OVMF.fd
endif

vscode_debug: build_kernel build_libc build_userspace build_image
	rm -f serial.log network.log
	$(QEMU) -S -gdb tcp::1234 -d int -no-shutdown $(QEMU_UEFI_BIOS) -m 4G $(QEMUFLAGS)

qemu: qemu_vdisk
	rm -f serial.log network.log
	$(QEMU) $(QEMU_UEFI_BIOS) -cpu host $(QEMUFLAGS) $(QEMUHWACCELERATION) $(QEMUMEMORY)

qemubios: qemu_vdisk
	rm -f serial.log network.log
	$(QEMU) -cpu host $(QEMUFLAGS) $(QEMUHWACCELERATION) $(QEMUMEMORY)

# build the os and run it
run: build qemu_vdisk qemu

# clean
clean:
	rm -rf doxygen-doc iso_tmp_data initrd.tar.gz *.iso
	make --quiet -C boot clean
	make --quiet -C kernel clean
	make --quiet -C userspace clean
	make --quiet -C libc clean
