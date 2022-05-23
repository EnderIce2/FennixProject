build:
	make -C init build
	make -C monoton build
	make -C uiserver build
	make -C wm build
	make -C fesktop build
ifneq (,$(wildcard doomgeneric))
	make -j$(shell nproc) -C doomgeneric build
else
	git clone https://github.com/EnderIce2/doomgeneric.git
	make -j$(shell nproc) -C doomgeneric build
endif


clean:
	make -C init clean
	make -C monoton clean
	make -C uiserver clean
	make -C wm clean
	make -C fesktop clean
ifneq (,$(wildcard doomgeneric))
	make -j$(shell nproc) -C doomgeneric clean
else
	git clone https://github.com/EnderIce2/doomgeneric.git
	make -j$(shell nproc) -C doomgeneric clean
endif
