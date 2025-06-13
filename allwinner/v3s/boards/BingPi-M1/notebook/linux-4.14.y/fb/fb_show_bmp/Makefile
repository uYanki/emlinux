

ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

testshow:testshow.c
	$(CROSS_COMPILE)gcc -Wall $< -o $@

clean:
	rm -f testshow *.o *~

install:
	sshpass -p '123456' scp testshow root@192.168.1.188:/root/
