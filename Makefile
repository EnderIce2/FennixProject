
include Makefile.conf

QEMUFLAGS = -device bochs-display -M q35 \
			-usb -no-reboot \
			-usbdevice mouse \
			-smp 1 \
			-serial file:serial.log \
			-soundhw pcspk \

QEMUHWACCELERATION = -machine q35 -enable-kvm

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
	make -C tools all
	make -C boot gnuefi

tools_workflow_all: fonts
	make -C tools workflow_all
	make -C boot gnuefi

build: build_bootloader build_kernel build_image

build_bootloader:
	make -C boot build

build_kernel:
	make -j$(shell nproc) -C kernel build GIT_COMMIT=$(shell git rev-parse HEAD) GIT_COMMIT_SHORT=$(shell git rev-parse --short HEAD)

build_image:
	mkdir -p iso_tmp_data
	cp kernel/kernel.fsys initrd.tar.gz startup.nsh \
		iso_tmp_data/
	cp lynx.cfg boot/BIOS/loader.bin boot/UEFI/efi-loader.bin iso_tmp_data/
	xorriso -as mkisofs -b loader.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot efi-loader.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_tmp_data -o $(OSNAME).iso

vscode_debug: build_kernel build_libc build_userspace build_image
	rm -f serial.log network.log
	${QEMU} -S -gdb tcp::1234 -d int -no-shutdown -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -m 512M ${QEMUFLAGS}

qemu:
	rm -f serial.log network.log
	${QEMU} -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -cpu host ${QEMUFLAGS} ${QEMUHWACCELERATION} ${QEMUMEMORY}

# build the os and run it
run: build qemu

# clean
clean:
	rm -rf iso_tmp_data *.iso
	make -C boot clean
	make -C kernel clean
