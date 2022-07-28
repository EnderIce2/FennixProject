build:
	make -C tictactoe build
ifneq (,$(wildcard doomgeneric))
	make -j$(shell nproc) -C doomgeneric build
else
	git clone https://github.com/EnderIce2/doomgeneric.git
	make -j$(shell nproc) -C doomgeneric build
endif

clean:
	make -C tictactoe clean
ifneq (,$(wildcard doomgeneric))
	make -j$(shell nproc) -C doomgeneric clean
else
	git clone https://github.com/EnderIce2/doomgeneric.git
	make -j$(shell nproc) -C doomgeneric clean
endif
