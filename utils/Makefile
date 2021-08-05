all:    make_font_table make_conv_table make_isalpha

make_isalpha: make_isalpha.c
	gcc -o $@ $<

make_font_table: make_font_table.c
	gcc -o $@ $<

make_conv_table: make_conv_table.c
	gcc -o $@ $<


clean:
	rm -rf make_font_table make_conv_table make_isalpha
