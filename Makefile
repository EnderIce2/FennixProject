
.PHONY: default tools clean

# first rule
default:
	$(info Hello! If you want to build the ISO & IMG file, please use the following command: make build)
	$(info If you want to quickly build and run in QEMU, please use the following command: make run)
	$(info If you want to clean, please use the following command: make clean)
	$(info Thanks.)

# install necessary packages, compile cross-compiler etc..
tools:
	make -C tools all

# build the operating system
build:
	make -C kernel build

# quickly build the operating system (it won't create the ISO file and doxygen documentation)
quick_build:
	make -C kernel build

# build the os and run it
run: quick_build

# clean
clean:
	make -C tools clean
	make -C kernel clean
