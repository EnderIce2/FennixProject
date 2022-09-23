
NASM = /usr/bin/nasm

build:
	$(NASM) boot.asm -f bin -o boot.bin
	$(NASM) second.asm -f bin -o second.bin
	cat boot.bin second.bin > loader.bin

clean:
	rm -f boot.bin second.bin loader.bin
