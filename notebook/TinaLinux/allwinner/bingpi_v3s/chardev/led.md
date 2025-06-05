# LED

## Device Tree

 `arch/arm/boot/dts/sun8i-v3s-licheepi-zero.dts` 

```D
leds {
		compatible = "gpio-leds";

		blue_led {
			label = "licheepi:blue:usr";
			gpios = <&pio 6 1 GPIO_ACTIVE_LOW>; /* PG1 */
		};

		green_led {
			label = "licheepi:green:usr";
			gpios = <&pio 6 0 GPIO_ACTIVE_LOW>; /* PG0 */
			default-state = "on";
		};

		red_led {
			label = "licheepi:red:usr";
			gpios = <&pio 6 2 GPIO_ACTIVE_LOW>; /* PG2 */
		};
	};
```

* notice

```D
gpios = <&pio 6 1 GPIO_ACTIVE_LOW>; /* PG1 */
```

pin：PG1（ A\~G：0\~6），6 * 32 + 1 = 193

name：licheepi:blue:usr

## Led Control

```shell
$ cd /sys/class/leds/
$ ls
licheepi:blue:usr   licheepi:green:usr  licheepi:red:usr

$ cd licheepi:blue:usr
$ ls
brightness      max_brightness  subsystem       uevent
device          power           trigger

$ echo 1 > brightness # on
$ echo 0 > brightness # off

$ echo heartbeat > trigger # blink
$ cat trigger 
none rc-feedback rfkill-any kbd-scrolllock kbd-numlock kbd-capslock kbd-kanalock kbd-shiftlock kbd-altgrlock kbd-ctrllock kbd-altlock kbd-shiftllock kbd-shiftrlock kbd-ctrlllock kbd-ctrlrlock mmc0 [heartbeat] default-on # seletced item:[]

$ echo none > trigger # off
$ echo default-on > trigger # on
```

