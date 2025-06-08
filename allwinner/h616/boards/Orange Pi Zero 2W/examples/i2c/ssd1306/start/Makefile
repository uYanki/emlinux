
all:  demo

demo: objects
	g++ demo/main.cpp obj/*.o -lwiringPi -o app


objects:  lib/ssd1306_i2c.c lib/ssd1306_i2c.h lib/oled_fonts.h
	@mkdir -p obj
	gcc -c lib/ssd1306_i2c.c -o obj/ssd1306_i2c.o -lm


clean:
	rm -f obj/*.o app
