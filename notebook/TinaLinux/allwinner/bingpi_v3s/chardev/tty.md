# TTY

```bash
$ cd /dev

$ ls ttyS*
ttyS0  ttyS1  ttyS2  ttyS3  ttyS4  ttyS5  ttyS6  ttyS7

# scan available serial ports 

$ dmesg | grep tty 
console [ttyS0] disabled
1c28000.serial: ttyS0 at MMIO 0x1c28000 (irq = 34, base_baud = 1500000) is a U6_16550A
console [ttyS0] enabled

$ cat /proc/tty/driver/serial
serinfo:1.0 driver revision:
0: uart:U6_16550A mmio:0x01C28000 irq:34 tx:63086 rx:4130 RTS|DTR
1: uart:unknown port:00000000 irq:0
2: uart:unknown port:00000000 irq:0
3: uart:unknown port:00000000 irq:0
4: uart:unknown port:00000000 irq:0
5: uart:unknown port:00000000 irq:0
6: uart:unknown port:00000000 irq:0
7: uart:unknown port:00000000 irq:0

$ echo hello > ttyS0 # send data
hello

$ cat ttyS0 # enter continuous transmission mode
....
$ ctrl+z # exit continuous transmission mode
```





