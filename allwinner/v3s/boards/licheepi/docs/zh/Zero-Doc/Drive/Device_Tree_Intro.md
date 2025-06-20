---
title: 设备树简介
---

Device Tree是一种描述硬件的数据结构，

DTS(Device Tree Source)就是用来描述目标板硬件信息的源文件。

## 设备树基本数据格式


device
tree是一个简单的节点和属性树，属性是键值对，节点可以包含属性和子节点。下面是一个.dts格式的简单设备树。

```
{  
    node1 {  
        a-string-property = "A string";  
        a-string-list-property = "first string", "second string";  
        a-byte-data-property = [0x01 0x23 0x34 0x56];  
        child-node1 {  
            first-child-property;  
            second-child-property = <1>;  
            a-string-property = "Hello, world";  
        };  
        child-node2 {  
        };  
    };  
    node2 {  
        an-empty-property;  
        a-cell-property = <1 2 3 4>; /* each number (cell) is a uint32 */  
        child-node1 {  
        };  
    };  
};  
```

编译设备树后，可以使用

`dtc -I dtb -O dts xxx.dtb -o xxx.dts`

来查看实际生成的设备树文件

在运行系统时，\*/sys/firmware/devicetree\* 可以查看实际使用的是设备树

## 设备树常用操作


```
/ {
    --- ---
    demo1: demo1 {
        compatible = "demo1";
        property1 = <1>;
        property2;
        property3 = <2>;
        property4;
    };
};

&demo1 {
    /delete-property/property2;
    /delete-property/property3;
};
```

```
memory_DDR1@c0000000 {
        device_type = "memory";
        reg = <0 0xc0000000 0 0x40000000>;
    };


/ {
    /delete-node/ memory_DDR1@c0000000;
};
```

## 设备树实例解析


下面解析sun8i-v3s.dtsi设备树实例

```
#include <dt-bindings/interrupt-controller/arm-gic.h>
#include <dt-bindings/clock/sun8i-v3s-ccu.h>
#include <dt-bindings/reset/sun8i-v3s-ccu.h>

/ {
    #address-cells = <1>;       //表示子节点默认用一个uint32表示地址（32位系统），如果是64位系统将是2
    #size-cells = <1>;      //表示子节点默认用一个uint32表示大小
    interrupt-parent = <&gic>;  //interrupt-parent是独立字段，表示整个设备的中断父节点是gic
    //<&gic>表示取gic地址。gic配置详见文件末尾

    chosen {
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        simplefb_lcd: framebuffer@0 {
            compatible = "allwinner,simple-framebuffer",
                    "simple-framebuffer";
            allwinner,pipeline = "de0-lcd0";
            clocks = <&ccu CLK_BUS_TCON0>, <&display_clocks 0>,
                <&display_clocks 6>, <&ccu CLK_TCON0>;
            status = "disabled";
        };
    };

    cpus {
        #address-cells = <1>;
        #size-cells = <0>;

        cpu@0 {
            compatible = "arm,cortex-a7";
            device_type = "cpu";
            reg = <0>;
            clocks = <&ccu CLK_CPU>;
        };
    };

    de: display-engine {
        compatible = "allwinner,sun8i-v3s-display-engine";
        allwinner,pipelines = <&mixer0>;
        status = "disabled";
    };

    timer {
        compatible = "arm,armv7-timer";
        interrupts = <GIC_PPI 13 (GIC_CPU_MASK_SIMPLE(4) | IRQ_TYPE_LEVEL_LOW)>,
                <GIC_PPI 14 (GIC_CPU_MASK_SIMPLE(4) | IRQ_TYPE_LEVEL_LOW)>,
                <GIC_PPI 11 (GIC_CPU_MASK_SIMPLE(4) | IRQ_TYPE_LEVEL_LOW)>,
                <GIC_PPI 10 (GIC_CPU_MASK_SIMPLE(4) | IRQ_TYPE_LEVEL_LOW)>;
    };

    clocks {
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        osc24M: osc24M_clk {
            #clock-cells = <0>;
            compatible = "fixed-clock";
            clock-frequency = <24000000>;
            clock-output-names = "osc24M";
        };

        osc32k: osc32k_clk {
            #clock-cells = <0>;
            compatible = "fixed-clock";
            clock-frequency = <32768>;
            clock-output-names = "osc32k";
        };
    };

    soc {
        compatible = "simple-bus";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        display_clocks: clock@1000000 {
            compatible = "allwinner,sun8i-v3s-de2-clk";
            reg = <0x01000000 0x100000>;
            clocks = <&ccu CLK_DE>,
                <&ccu CLK_BUS_DE>;
            clock-names = "mod",
                    "bus";
            resets = <&ccu RST_BUS_DE>;
            #clock-cells = <1>;
            #reset-cells = <1>;
        };

        mixer0: mixer@1100000 {
            compatible = "allwinner,sun8i-v3s-de2-mixer";
            reg = <0x01100000 0x100000>;
            clocks = <&display_clocks 0>,
                <&display_clocks 6>;
            clock-names = "bus",
                    "mod";
            resets = <&display_clocks 0>;
            assigned-clocks = <&display_clocks 6>;
            assigned-clock-rates = <150000000>;

            ports {
                #address-cells = <1>;
                #size-cells = <0>;

                mixer0_out: port@1 {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    reg = <1>;

                    mixer0_out_tcon0: endpoint@0 {
                        reg = <0>;
                        remote-endpoint = <&tcon0_in_mixer0>;
                    };
                };
            };
        };

        tcon0: lcd-controller@1c0c000 {
            compatible = "allwinner,sun8i-v3s-tcon";
            reg = <0x01c0c000 0x1000>;
            interrupts = <GIC_SPI 86 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_TCON0>,
                <&ccu CLK_TCON0>;
            clock-names = "ahb",
                    "tcon-ch0";
            clock-output-names = "tcon-pixel-clock";
            resets = <&ccu RST_BUS_TCON0>;
            reset-names = "lcd";
            status = "disabled";

            ports {
                #address-cells = <1>;
                #size-cells = <0>;

                tcon0_in: port@0 {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    reg = <0>;

                    tcon0_in_mixer0: endpoint@0 {
                        reg = <0>;
                        remote-endpoint = <&mixer0_out_tcon0>;
                    };
                };

                tcon0_out: port@1 {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    reg = <1>;
                };
            };
        };


        mmc0: mmc@01c0f000 {
            compatible = "allwinner,sun7i-a20-mmc";
            reg = <0x01c0f000 0x1000>;
            clocks = <&ccu CLK_BUS_MMC0>,
                <&ccu CLK_MMC0>,
                <&ccu CLK_MMC0_OUTPUT>,
                <&ccu CLK_MMC0_SAMPLE>;
            clock-names = "ahb",
                    "mmc",
                    "output",
                    "sample";
            resets = <&ccu RST_BUS_MMC0>;
            reset-names = "ahb";
            interrupts = <GIC_SPI 60 IRQ_TYPE_LEVEL_HIGH>;
            status = "disabled";
            #address-cells = <1>;
            #size-cells = <0>;
        };

        mmc1: mmc@01c10000 {
            compatible = "allwinner,sun7i-a20-mmc";
            reg = <0x01c10000 0x1000>;
            clocks = <&ccu CLK_BUS_MMC1>,
                <&ccu CLK_MMC1>,
                <&ccu CLK_MMC1_OUTPUT>,
                <&ccu CLK_MMC1_SAMPLE>;
            clock-names = "ahb",
                    "mmc",
                    "output",
                    "sample";
            resets = <&ccu RST_BUS_MMC1>;
            reset-names = "ahb";
            interrupts = <GIC_SPI 61 IRQ_TYPE_LEVEL_HIGH>;
            pinctrl-names = "default";
            pinctrl-0 = <&mmc1_pins>;
            status = "disabled";
            #address-cells = <1>;
            #size-cells = <0>;
        };

        mmc2: mmc@01c11000 {
            compatible = "allwinner,sun7i-a20-mmc";
            reg = <0x01c11000 0x1000>;
            clocks = <&ccu CLK_BUS_MMC2>,
                <&ccu CLK_MMC2>,
                <&ccu CLK_MMC2_OUTPUT>,
                <&ccu CLK_MMC2_SAMPLE>;
            clock-names = "ahb",
                    "mmc",
                    "output",
                    "sample";
            resets = <&ccu RST_BUS_MMC2>;
            reset-names = "ahb";
            interrupts = <GIC_SPI 62 IRQ_TYPE_LEVEL_HIGH>;
            status = "disabled";
            #address-cells = <1>;
            #size-cells = <0>;
        };

        usb_otg: usb@01c19000 {
            compatible = "allwinner,sun8i-h3-musb";
            reg = <0x01c19000 0x0400>;
            clocks = <&ccu CLK_BUS_OTG>;
            resets = <&ccu RST_BUS_OTG>;
            interrupts = <GIC_SPI 71 IRQ_TYPE_LEVEL_HIGH>;
            interrupt-names = "mc";
            phys = <&usbphy 0>;
            phy-names = "usb";
            extcon = <&usbphy 0>;
            status = "disabled";
        };

        usbphy: phy@01c19400 {
            compatible = "allwinner,sun8i-v3s-usb-phy";
            reg = <0x01c19400 0x2c>,
                <0x01c1a800 0x4>;
            reg-names = "phy_ctrl",
                    "pmu0";
            clocks = <&ccu CLK_USB_PHY0>;
            clock-names = "usb0_phy";
            resets = <&ccu RST_USB_PHY0>;
            reset-names = "usb0_reset";
            status = "disabled";
            #phy-cells = <1>;
        };

        ehci0: usb@01c1a000 {
            compatible = "allwinner,sun8i-v3s-ehci", "generic-ehci";
            reg = <0x01c1a000 0x100>;
            interrupts = <GIC_SPI 72 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_EHCI0>, <&ccu CLK_BUS_OHCI0>;
            resets = <&ccu RST_BUS_EHCI0>, <&ccu RST_BUS_OHCI0>;
            status = "disabled";
        };

        ohci0: usb@01c1a400 {
            compatible = "allwinner,sun8i-v3s-ohci", "generic-ohci";
            reg = <0x01c1a400 0x100>;
            interrupts = <GIC_SPI 73 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_EHCI0>, <&ccu CLK_BUS_OHCI0>,
                <&ccu CLK_USB_OHCI0>;
            resets = <&ccu RST_BUS_EHCI0>, <&ccu RST_BUS_OHCI0>;
            status = "disabled";
        };

        ccu: clock@01c20000 {
            compatible = "allwinner,sun8i-v3s-ccu";
            reg = <0x01c20000 0x400>;
            clocks = <&osc24M>, <&osc32k>;
            clock-names = "hosc", "losc";
            #clock-cells = <1>;
            #reset-cells = <1>;
        };

        rtc: rtc@01c20400 {
            compatible = "allwinner,sun6i-a31-rtc";
            reg = <0x01c20400 0x54>;
            interrupts = <GIC_SPI 40 IRQ_TYPE_LEVEL_HIGH>,
                    <GIC_SPI 41 IRQ_TYPE_LEVEL_HIGH>;
        };

        pio: pinctrl@01c20800 {
            compatible = "allwinner,sun8i-v3s-pinctrl";
            reg = <0x01c20800 0x400>;
            interrupts = <GIC_SPI 15 IRQ_TYPE_LEVEL_HIGH>,
                    <GIC_SPI 17 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_PIO>, <&osc24M>, <&osc32k>;
            clock-names = "apb", "hosc", "losc";
            gpio-controller;
            #gpio-cells = <3>;
            interrupt-controller;
            #interrupt-cells = <3>;

            i2c0_pins: i2c0 {
                pins = "PB6", "PB7";
                function = "i2c0";
            };

            uart0_pins_a: uart0@0 {
                pins = "PB8", "PB9";
                function = "uart0";
            };

            mmc0_pins_a: mmc0@0 {
                pins = "PF0", "PF1", "PF2", "PF3",
                    "PF4", "PF5";
                function = "mmc0";
                drive-strength = <30>;
                bias-pull-up;
            };

            mmc1_pins: mmc1 {
                pins = "PG0", "PG1", "PG2", "PG3",
                    "PG4", "PG5";
                function = "mmc1";
                drive-strength = <30>;
                bias-pull-up;
            };

            spi0_pins: spi0 {
                pins = "PC0", "PC1", "PC2", "PC3";
                function = "spi0";
            };
        };

        timer@01c20c00 {
            compatible = "allwinner,sun4i-a10-timer";
            reg = <0x01c20c00 0xa0>;
            interrupts = <GIC_SPI 18 IRQ_TYPE_LEVEL_HIGH>,
                    <GIC_SPI 19 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&osc24M>;
        };

        wdt0: watchdog@01c20ca0 {
            compatible = "allwinner,sun6i-a31-wdt";
            reg = <0x01c20ca0 0x20>;
            interrupts = <GIC_SPI 25 IRQ_TYPE_LEVEL_HIGH>;
        };

        lradc: lradc@1c22800 {
            compatible = "allwinner,sun4i-a10-lradc-keys";
            reg = <0x01c22800 0x400>;
            interrupts = <GIC_SPI 30 IRQ_TYPE_LEVEL_HIGH>;
            status = "disabled";
        };

        uart0: serial@01c28000 {
            compatible = "snps,dw-apb-uart";
            reg = <0x01c28000 0x400>;
            interrupts = <GIC_SPI 0 IRQ_TYPE_LEVEL_HIGH>;
            reg-shift = <2>;
            reg-io-width = <4>;
            clocks = <&ccu CLK_BUS_UART0>;
            resets = <&ccu RST_BUS_UART0>;
            status = "disabled";
        };

        uart1: serial@01c28400 {
            compatible = "snps,dw-apb-uart";
            reg = <0x01c28400 0x400>;
            interrupts = <GIC_SPI 1 IRQ_TYPE_LEVEL_HIGH>;
            reg-shift = <2>;
            reg-io-width = <4>;
            clocks = <&ccu CLK_BUS_UART1>;
            resets = <&ccu RST_BUS_UART1>;
            status = "disabled";
        };

        uart2: serial@01c28800 {
            compatible = "snps,dw-apb-uart";
            reg = <0x01c28800 0x400>;
            interrupts = <GIC_SPI 2 IRQ_TYPE_LEVEL_HIGH>;
            reg-shift = <2>;
            reg-io-width = <4>;
            clocks = <&ccu CLK_BUS_UART2>;
            resets = <&ccu RST_BUS_UART2>;
            status = "disabled";
        };

        i2c0: i2c@01c2ac00 {
            compatible = "allwinner,sun6i-a31-i2c";
            reg = <0x01c2ac00 0x400>;
            interrupts = <GIC_SPI 6 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_I2C0>;
            resets = <&ccu RST_BUS_I2C0>;
            pinctrl-names = "default";
            pinctrl-0 = <&i2c0_pins>;
            status = "disabled";
            #address-cells = <1>;
            #size-cells = <0>;
        };

        i2c1: i2c@01c2b000 {
            compatible = "allwinner,sun6i-a31-i2c";
            reg = <0x01c2b000 0x400>;
            interrupts = <GIC_SPI 7 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_I2C1>;
            resets = <&ccu RST_BUS_I2C1>;
            status = "disabled";
            #address-cells = <1>;
            #size-cells = <0>;
        };

        spi0: spi@1c68000 {
            compatible = "allwinner,sun8i-h3-spi";
            reg = <0x01c68000 0x1000>;
            interrupts = <GIC_SPI 65 IRQ_TYPE_LEVEL_HIGH>;
            clocks = <&ccu CLK_BUS_SPI0>, <&ccu CLK_SPI0>;
            clock-names = "ahb", "mod";
            pinctrl-names = "default";
            pinctrl-0 = <&spi0_pins>;
            resets = <&ccu RST_BUS_SPI0>;
            status = "disabled";
            #address-cells = <1>;
            #size-cells = <0>;
        };

        gic: interrupt-controller@01c81000 {    // interrupt-controller是独立字段，表示中断控制器。gic是其一个实例，地址在01c81000
            compatible = "arm,cortex-a7-gic", "arm,cortex-a15-gic"; //驱动匹配
            reg = <0x01c81000 0x1000>,  //中断控制器的寄存器地址，和大小
                <0x01c82000 0x1000>,
                <0x01c84000 0x2000>,
                <0x01c86000 0x2000>;
            interrupt-controller;
            #interrupt-cells = <3>; //调用该实例需要三个参数
            interrupts = <GIC_PPI 9 (GIC_CPU_MASK_SIMPLE(4) | IRQ_TYPE_LEVEL_HIGH)>;
        };
    };
};
```

系统中的每个设备由device tree的一个节点来表示；

```
static struct of_device_id beep_table[] = {  
    {.compatible = "fs4412,beep"},  
};  
static struct platform_driver beep_driver=  
{  
    .probe = beep_probe,  
    .remove = beep_remove,  
    .driver={  
        .name = "bigbang",  
        .of_match_table = beep_table,  
    },  
};
```

```
fs4412-beep{  
        compatible = "fs4412,beep";  
        reg = <0x114000a0 0x4 0x139D0000 0x14>;  
};  
```

**compatible**，关键属性，驱动中使用of_match_table，即of_device_id列表，其中就使用compatible字段来匹配设备。
   简单地说就是，内核启动后会把设备树加载到树状结构体中，当insmod的时候，就会在树中查找匹配的设备节点来加载。

**reg**，描述寄存器基址和长度，可以有多个。

```
pwm: pwm@01c21400 {
            compatible = "allwinner,sun8i-h3-pwm";  
            reg = <0x01c21400 0x8>;
            clocks = <&osc24M>;
            #pwm-cells = <3>;
            status = "okay";
    };
```

    实例 : 类 @ 地址 {
    compatible： “供应商, 设备id”
    reg
    clocks
    }

## 基于设备树的driver的结构体的填充


```
static struct of_device_id beep_table[] = {  
    {.compatible = "fs4412,beep"},  
};  
static struct platform_driver beep_driver=  
{  
    .probe = beep_probe,  
    .remove = beep_remove,  
    .driver={  
        .name = "bigbang",  
        .of_match_table = beep_table,  
    },  
};
```

make dtbs 在内核根目录

## sys下设备树查看


**/sys/firmware/devicetree** 可以查看实际使用的是设备树

## 参考资料

<http://e76103db.wiz03.com/share/s/3Dogfr2DXA6Q27Og6x2QoKY92Bk9at14FABh21OAAE28lAme>
