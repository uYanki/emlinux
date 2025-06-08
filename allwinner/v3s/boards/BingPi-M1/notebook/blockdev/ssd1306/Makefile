ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

ssd1306_show_image:ssd1306_show_image.c
	$(CROSS_COMPILE)gcc -Wall $< -o $@

clean:
	rm -f ssd1306_show_image *.o *~

install:
	sshpass -p '123456' scp ssd1306_show_image root@192.168.1.188:/root/