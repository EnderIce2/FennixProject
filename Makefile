
include Makefile.conf

QEMUFLAGS = -device bochs-display -M q35 \
			-usb -no-reboot \
			-usbdevice mouse \
			-smp $(shell nproc) \
    		-netdev user,id=usernet0 \
    		-device e1000,netdev=usernet0,mac=00:69:96:00:42:00 \
			-object filter-dump,id=usernet0,netdev=usernet0,file=network.log,maxlen=1024 \
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

tools_workflow: fonts
	make -C tools workflow

tools_workflow_all: fonts
	make -C tools workflow_all

build: build_kernel build_libc build_userspace build_image

build_kernel:
	make -j$(shell nproc) -C kernel build GIT_COMMIT=$(shell git rev-parse HEAD) GIT_COMMIT_SHORT=$(shell git rev-parse --short HEAD)

build_userspace:
	make -C userspace build

build_libc:
	make -C libc build

build_image:
	mkdir -p iso_tmp_data
	tar cf initrd.tar.gz -C resources/initrd/ ./ --format=ustar
	cp kernel/kernel.fsys initrd.tar.gz startup.nsh \
		iso_tmp_data/
	cp limine.cfg ${LIMINE_FOLDER}/limine.sys ${LIMINE_FOLDER}/limine-cd.bin ${LIMINE_FOLDER}/limine-cd-efi.bin iso_tmp_data/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_tmp_data -o $(OSNAME).iso

vscode_debug: build_kernel build_libc build_userspace build_image
	rm -f serial.log network.log
	${QEMU} -S -gdb tcp::1234 -d int -no-shutdown -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -m 4G ${QEMUFLAGS}

qemu: qemu_vdisk
	rm -f serial.log network.log
	${QEMU} -drive file=$(OSNAME).iso -bios /usr/share/qemu/OVMF.fd -cpu host ${QEMUFLAGS} ${QEMUHWACCELERATION} ${QEMUMEMORY}

# build the os and run it
run: build qemu_vdisk qemu

# clean
clean:
	rm -rf iso_tmp_data initrd.tar.gz *.iso
	make -C kernel clean
	make -C userspace clean
	make -C libc clean
