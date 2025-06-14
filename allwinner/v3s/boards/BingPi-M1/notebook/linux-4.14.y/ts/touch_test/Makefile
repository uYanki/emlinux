ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

touch_test:touch_test.c
	$(CROSS_COMPILE)gcc -Wall $< -o $@

clean:
	rm -f touch_test *.o *~

install:
	sshpass -p '123456' scp touch_test root@192.168.1.188:/root/
