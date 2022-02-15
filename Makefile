all: c64
# c128

SERIAL=/dev/ttyUSB4
ACIACONFIG= -acia1 -acia1mode 2 -acia1irq 1 -acia1base 0xde00 -rsdev1baud 38400 -rsdev2 $(SERIAL)


c64: autogen
	mkdir -p build-c64
	cd build-c64; make -f ../Makefile.c64

#c128:
#	mkdir -p build-c128
#	cd build-c128; make -f ../Makefile.c128

clean:
	rm -rf build-c64 build-c128 src/soft80-font.c src/ascii_define.h src/char_tables.c gterm_bin.tar.gz
	cd utils; make clean

bin: all
	tar cvfz gterm_bin.tar.gz \
		build-c64/*.bin  build-c64/*.crt  build-c64/gterm64

autogen: tools
	utils/make_font_table charset/soft80-charset.bin >src/soft80-font.c
	utils/make_conv_table h >src/ascii_define.h
	utils/make_conv_table c >src/char_tables.c
	utils/make_isalpha >> src/char_tables.c


test: all
	x64 -cartcrt build-c64/gterm64.crt $(ACIACONFIG)

#	x128 -extfrom build-c128/cterm128.bin -extfunc 1

tools:
	cd utils; make

cart: all
	cat build-c64/gmt64.bin build-c64/gmt64.bin build-c64/gram1-64.bin build-c64/gram4-64.bin \
		build-c128/gmt128.bin build-c128/gmt128.bin build-c128/gram1-128.bin build-c128/gram4-128.bin \
		> cart.bin
