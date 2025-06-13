ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

dht11:dht11.c
	$(CROSS_COMPILE)gcc -Wall $< -o $@

clean:
	rm -f dht11 *.o *~

install:
	sshpass -p '123456' scp dht11 root@192.168.1.188:/root/
