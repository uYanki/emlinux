# GPIO

```bash
# PG3
$ echo 195 > /sys/class/gpio/export # export

$ cd /sys/class/gpio/
$ ls
export    gpio195    gpiochip0  unexport

$ cd gpio195
$ ls
active_low  direction   power       uevent
device      edge        subsystem   value

$ echo out > direction # set dir: output
$ echo 0 > value # set level: low 
$ echo 1 > value # set level: high 

$ echo in > direction # set dir: input
$ cat value 
0 # connect to GND
$ cat value
1 # connect to 3V3

$ echo 195 > /sys/class/gpio/unexport # unexport
```

