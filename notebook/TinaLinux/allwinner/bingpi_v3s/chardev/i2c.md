# I2C

```bash
$ ls /dev/i2c* # scan i2cbus
/dev/i2c-0

$ i2cdetect
Usage: i2cdetect -l | -F I2CBUS | [-ya] [-q|-r] I2CBUS [FIRST LAST]
Detect I2C chips
        -l      List installed buses
        -F BUS# List functionalities on this bus
        -y      Disable interactive mode
        -a      Force scanning of non-regular addresses
        -q      Use smbus quick write commands for probing (default)
        -r      Use smbus read byte commands for probing
        FIRST and LAST limit probing range

$ i2cdetect -y -r 0 # scan device address
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- 48 -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
# notice: ns2009 address = 0x48
```

