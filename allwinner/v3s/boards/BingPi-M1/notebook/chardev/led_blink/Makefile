ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

led_blink:led_blink.c
	$(CROSS_COMPILE)gcc -Wall $< -o $@

clean:
	rm -f led_blink *.o *~

install:
	sshpass -p '123456' scp led_blink root@192.168.1.188:/root/