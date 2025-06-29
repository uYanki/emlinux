---
title: SPI Flash 系统编译
---

在一些低成本应用场景，需要在SPI
flash上启动系统，这需要对Uboot和系统镜像做些适配。

本文介绍SPI Flash镜像的制作过程。

## Flash分区规划


这里 使用 MX25L25645G, **32M SPI flash** 作为启动介质，规划分区如下：

## Uboot编译配置


由于目前Uboot环境变量固定存放在1MB位置之内，所有留给uboot的空间固定到flash前1MB的位置不变。

每个分区的大小必须是擦除块大小的整数倍，MX25L25645G的擦除块大小是64KB。

### 准备uboot


下载包含spi驱动的体验版本uboot，该驱动目前尚未合并到主线

> `git clone -b v3s-spi-experimental https://github.com/Lichee-Pi/u-boot.git`

### 配置Flash支持型号


执行 `make ARCH=arm menuconfig` 打开uboot菜单配置，进入到
Device Drivers --\> SPI Flash Support

注意看一下自己flash的厂家名称，例如选上Macronix SPI flash
support用来支持测试用的flash：MX25L25645G。

如果使用的是16MB以上的flash，需要勾选flash
bank支持选项，否则最多只能读到16MB： **CONFIG\_SPI\_FLASH\_BAR**

### 配置uboot默认环境变量


在文件 **include/configs/sun8i.h**
中添加默认bootcmd和bootargs的环境变量设置，注意添加的位置在“#include &#60;configs/sunxi-common.h&#62; ”的前边。

![](./../static/System_Development/uboot_conf_4.jpg)


```
#define CONFIG_BOOTCOMMAND   "sf probe 0; "                           \
                            "sf read 0x41800000 0x100000 0x10000; "  \
                            "sf read 0x41000000 0x110000 0x400000; " \
                            "bootz 0x41000000 - 0x41800000"

#define CONFIG_BOOTARGS      "console=ttyS0,115200 earlyprintk panic=5 rootwait " \
                            "mtdparts=spi32766.0:1M(uboot)ro,64k(dtb)ro,4M(kernel)ro,-(rootfs) root=31:03 rw rootfstype=jffs2"
```

环境命令解析：

- sf probe 0; //初始化Flash设备（CS拉低）
- sf read 0x41800000 0x100000 0x10000;//从flash0x100000（1MB）位置读取dtb放到内存0x41800000偏移处。//如果是bsp的bin，则是0x41d00000
- sf read 0x41000000 0x110000 0x400000;//从flash0x110000（1MB+64KB）位置读取dtb放到内存0x41000000偏移处。
- bootz 0x41000000 （内核地址）- 0x41800000（dtb地址） 启动内核

启动参数解析:
-   console=ttyS0,115200 earlyprintk panic=5 rootwait//在串口0上输出信息
-   mtdparts=spi32766.0:1M(uboot)ro,64k(dtb)ro,4M(kernel)ro,-(rootfs) root=31:03 rw rootfstype=jffs2 //spi32766.0是设备名，后面是分区大小，名字，读写属性。
-   root=31:03表示根文件系统是mtd3；jffs2格式

### 编译uboot


```
time make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- 2>&1 | tee build.log
```

会在目录下生成 *u-boot-sunxi-with-spl.bin*

## linux内核编译配置

linux内核基于github上的版<a href="https://github.com/Lichee-Pi/linux.git" target="_blank">https://github.com/Lichee-Pi/linux.git</a>，分支为最新的zero-4.13.y

### 内核选项配置

执行 `make ARCH=arm menuconfig` 打开内核菜单配置，

进入到 Device Drivers --> Memory Technology Device (MTD) support ，

确保选择上mtd的 **<*> Command line partition table parsing**
支持，该项目用来解析uboot传递过来的flash分区信息。

以及SPI-NOR 设备的支持。

![](./../static/System_Development/uboot_conf_5.jpg)


添加对jffs2文件系统的支持，路径在
File systems --\> Miscellaneous filesystems --\> Journalling Flash File System v2 (JFFS2) support

![](./../static/System_Development/uboot_conf_6.png)


### 设备树配置


修改dts配置添加spi flash节点

> `vi arch/arm/boot/dts/sun8i-v3s-licheepi-zero.dts`

添加spi节点配置:

```
&spi0 {
        status ="okay";
        mx25l25635e:mx25l25635e@0 {
                compatible = "jedec,spi-nor";
                reg = <0x0>;
                spi-max-frequency = <50000000>;
                #address-cells = <1>;
                #size-cells = <1>;
        };
};
```

![](./../static/System_Development/uboot_conf_7.jpg)


这里的flash型号需要在下表之中，否则将无法识别：（注意容量也一定要对应）

```
static const struct spi_device_id m25p_ids[] = {
        /*
        * Allow non-DT platform devices to bind to the "spi-nor" modalias, and
        * hack around the fact that the SPI core does not provide uevent
        * matching for .of_match_table
        */
        {"spi-nor"},

        /*
        * Entries not used in DTs that should be safe to drop after replacing
        * them with "spi-nor" in platform data.
        */
        {"s25sl064a"},  {"w25x16"},     {"m25p10"},     {"m25px64"},

        /*
        * Entries that were used in DTs without "jedec,spi-nor" fallback and
        * should be kept for backward compatibility.
        */
        {"at25df321a"}, {"at25df641"},  {"at26df081a"},
        {"mx25l4005a"}, {"mx25l1606e"}, {"mx25l6405d"}, {"mx25l12805d"},
        {"mx25l25635e"},{"mx66l51235l"},
        {"n25q064"},    {"n25q128a11"}, {"n25q128a13"}, {"n25q512a"},
        {"s25fl256s1"}, {"s25fl512s"},  {"s25sl12801"}, {"s25fl008k"},
        {"s25fl064k"},
        {"sst25vf040b"},{"sst25vf016b"},{"sst25vf032b"},{"sst25wf040"},
        {"m25p40"},     {"m25p80"},     {"m25p16"},     {"m25p32"},
        {"m25p64"},     {"m25p128"},
        {"w25x80"},     {"w25x32"},     {"w25q32"},     {"w25q32dw"},
        {"w25q80bl"},   {"w25q128"},    {"w25q256"},

        /* Flashes that can't be detected using JEDEC */
        {"m25p05-nonjedec"},    {"m25p10-nonjedec"},    {"m25p20-nonjedec"},
        {"m25p40-nonjedec"},    {"m25p80-nonjedec"},    {"m25p16-nonjedec"},
        {"m25p32-nonjedec"},    {"m25p64-nonjedec"},    {"m25p128-nonjedec"},

        /* Everspin MRAMs (non-JEDEC) */
        { "mr25h256" }, /* 256 Kib, 40 MHz */
        { "mr25h10" },  /*   1 Mib, 40 MHz */
        { "mr25h40" },  /*   4 Mib, 40 MHz */

        { },
};
```

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j32
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs
```

## 准备镜像文件


### 下载根文件系统


首先选择rootfs文件系统，我是用的是群朋提供的最小根文件系统
*rootfs-brmin.tar.gz*，大小只有3M左右，下载地址在

    https://pan.baidu.com/share/link?
    shareid=1432204556&uk=3658413294#list/path=%2F
    Lichee Zero>zero_imager.zip

### 制作jffs2文件系统


Flash支持jffs2文件系统格式，所以需要使用此该rootfs制作jffs2文件系统镜像、

下载jffs2文件系统制作工具

> `apt-get install mtd-utils`

解压 rootfs-brmin.tar.gz

> `tar xzvf rootfs-brmin.tar.gz`

计算好jffs的大小，可以使用zero\_imager里的 **make\_jffs2.sh 32** 生成

总空间是32M-1M-64K-4M=0x1AF0000

> `mkfs.jffs2 -s 0x100 -e 0x10000 -p 0x1AF0000 -d rootfs/ -o jffs2.img`

- 页大小0x100 256字节
- 块大小0x10000 64k
- jffs2分区总空间0x1AF0000
- jffs2.img是生成的文件系统镜像。

最后将uboot，dtb，kernel，rootfs打包成一个系统镜像，命令如下;（即zero_imager里的 *make_spiflash.sh 32 dock*）

```
#!/bin/sh
dd if=/dev/zero of=flashimg.bin bs=1M count=$1
dd if=u-boot-sunxi-with-spl-$2.bin of=flashimg.bin bs=1K conv=notrunc
dd if=sun8i-v3s-licheepi-zero-$2.dtb of=flashimg.bin bs=1K seek=1024  conv=notrunc
dd if=zImage of=flashimg.bin bs=1K seek=1088  conv=notrunc
dd if=jffs2.img of=flashimg.bin  bs=1K seek=5184  conv=notrunc
```

第一步： 生成一个空文件，大小是32MB\
第二步： 将uboot添加到文件开头\
第三步： 将dtb放到1M偏移处\
第四步： 将kernel放到1M+64K偏移处
第五步： 将rootfs放到1M+64K+4M偏移处

偏移大小是seek，单位是KB。

执行完毕后生成镜像文件 **flashimg.bin**

## 烧写镜像


### 下载sunxiflash烧写工具

    git clone -b spi-rebase https://github.com/Icenowy/sunxi-tools.git

> 注意是spi-rebase分支。

进入工具目录执行 `make && sudo make install`

如果出现：\*fel\_lib.c:26:20: fatal error: libusb.h: No such file or directory\*，那需要安装libusb：

`sudo apt-get install libusb-1.0-0-dev`

### 进入fel模式


Zero有一个usb下载模式称为fel模式，进入fel模式有下面几种方式：

1. TF卡和spi flash 同时没有可启动镜像
也就是说你不插卡，且焊接的是新的或者没有有效镜像的spi flash，那就上电自动进入fel下载模式

2.  TF卡中有进入fel模式的特殊固件 **fel-sdboot.sunxi**
如果你的spiflash已经有了启动镜像，那么需要在TF卡中烧入一个sunxi提供的启动工具 （`dd if=fel-sdboot.sunxi of=/dev/mmcblk0 bs=1024 seek=8` ），那么插入该TF卡启动会进入fel模式；

3.  上电时SPI_MISO拉低到地
该引脚为boot引脚，上电时出于低电平即会进入fel下载模式。

### sunxi-fel的操作


进入fel模式后使用usb数据线连接pc和zero,即可进行操作。

```
sudo sunxi-fel version      #查看连接的cpu信息
AWUSBFEX soc=00001681(V3s) 00000001 ver=0001 44 08 scratchpad=00007e00 00000000 00000000
sudo sunxi-fel spiflash-info    #显示flash信息
Manufacturer: Unknown (C2h), model: 20h, size: 33554432 bytes.
```

执行如下命令烧入我们前边制作好的镜像文件

```
sudo sunxi-fel -p spiflash-write 0 flashimg.bin
# -p 显示进度条
#   spiflash-info           Retrieves basic information
#   spiflash-hex[dump] addr length  Dumps SPI flash region in hex
#   spiflash-read addr length file  Write SPI flash contents into file
#   spiflash-write addr file    Store file contents into SPI flash
```

![](./../static/System_Development/uboot_conf_8.jpg)


SPI
flash下载速度约50KB/s，等待5分钟（16MB）或者10分钟（32MB），烧写完成，如果一切顺利，重新上电zero那么就会进入linux系统了，账号是root没有密码。

![](./../static/System_Development/uboot_conf_9.jpg)


## 附录： 启动日志

    U-Boot SPL 2017.01-rc2-00073-gdd6e874-dirty (Oct 14 2017 - 16:33:01)
    DRAM: 64 MiB
    Trying to boot from sunxi SPI

    U-Boot 2017.01-rc2-00073-gdd6e874-dirty (Oct 14 2017 - 16:33:01 +0000) Allwinner Technology

    CPU:   Allwinner V3s (SUN8I 1681)
    Model: Lichee Pi Zero
    DRAM:  64 MiB
    MMC:   SUNXI SD/MMC: 0
    SF: Detected mx25l25635f with page size 256 Bytes, erase size 64 KiB, total 32 MiB
    *** Warning - bad CRC, using default environment

    Setting up a 800x480 lcd console (overscan 0x0)
    dotclock: 33000kHz = 33000kHz: (1 * 3MHz * 66) / 6
    In:    serial@01c28000
    Out:   serial@01c28000
    Err:   serial@01c28000


    U-Boot 2017.01-rc2-00073-gdd6e874-dirty (Oct 14 2017 - 16:33:01 +0000) Allwinner Technology

    CPU:   Allwinner V3s (SUN8I 1681)
    Model: Lichee Pi Zero
    DRAM:  64 MiB
    MMC:   SUNXI SD/MMC: 0
    SF: Detected mx25l25635f with page size 256 Bytes, erase size 64 KiB, total 32 MiB
    *** Warning - bad CRC, using default environment

    Setting up a 800x480 lcd console (overscan 0x0)
    dotclock: 33000kHz = 33000kHz: (1 * 3MHz * 66) / 6
    In:    serial@01c28000
    Out:   serial@01c28000
    Err:   serial@01c28000
    Net:   No ethernet found.
    starting USB---
    No controllers found
    Hit any key to stop autoboot:  0 
    SF: Detected mx25l25635f with page size 256 Bytes, erase size 64 KiB, total 32 MiB
    device 0 offset 0x100000, size 0x10000
    SF: 65536 bytes @ 0x100000 Read: OK
    device 0 offset 0x110000, size 0x400000
    SF: 4194304 bytes @ 0x110000 Read: OK
    ## Flattened Device Tree blob at 41800000
    Booting using the fdt blob at 0x41800000
    Loading Device Tree to 42dfa000, end 42dffc48 --- OK

    Starting kernel ---
    [    0.000000] Booting Linux on physical CPU 0x0
    [    0.000000] Linux version 4.13.0-licheepi-zero+ (root@bf756b445919) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05)) #10 SMP Sat Oct 14 16:59:37 UTC 2017
    [    0.000000] CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=10c5387d
    [    0.000000] CPU: div instructions available: patching division code
    [    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
    [    0.000000] OF: fdt: Machine model: Lichee Pi Zero XFJ
    [    0.000000] Memory policy: Data cache writealloc
    [    0.000000] percpu: Embedded 16 pages/cpu @c3de6000 s33868 r8192 d23476 u65536
    [    0.000000] Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 15883
    [    0.000000] Kernel command line: console=ttyS0,115200 earlyprintk panic=5 rootwait mtdparts=spi32766.0:1M(uboot)ro,64k(dtb)ro,4M(kernel)ro,-(rootfs) root=31:03 rw rootfstype=jffs2
    [    0.000000] PID hash table entries: 256 (order: -2, 1024 bytes)
    [    0.000000] Dentry cache hash table entries: 8192 (order: 3, 32768 bytes)
    [    0.000000] Inode-cache hash table entries: 4096 (order: 2, 16384 bytes)
    [    0.000000] Memory: 53576K/64036K available (6144K kernel code, 229K rwdata, 1512K rodata, 1024K init, 265K bss, 10460K reserved, 0K cma-reserved, 0K highmem)
    [    0.000000] Virtual kernel memory layout:
    [    0.000000]     vector  : 0xffff0000 - 0xffff1000   (   4 kB)
    [    0.000000]     fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
    [    0.000000]     vmalloc : 0xc4000000 - 0xff800000   ( 952 MB)
    [    0.000000]     lowmem  : 0xc0000000 - 0xc3e89000   (  62 MB)
    [    0.000000]     pkmap   : 0xbfe00000 - 0xc0000000   (   2 MB)
    [    0.000000]     modules : 0xbf000000 - 0xbfe00000   (  14 MB)
    [    0.000000]       .text : 0xc0008000 - 0xc0700000   (7136 kB)
    [    0.000000]       .init : 0xc0900000 - 0xc0a00000   (1024 kB)
    [    0.000000]       .data : 0xc0a00000 - 0xc0a39580   ( 230 kB)
    [    0.000000]        .bss : 0xc0a3f65c - 0xc0a81b54   ( 266 kB)
    [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
    [    0.000000] Hierarchical RCU implementation.
    [    0.000000]  RCU event tracing is enabled.
    [    0.000000]  RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=1.
    [    0.000000] RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=1
    [    0.000000] NR_IRQS: 16, nr_irqs: 16, preallocated irqs: 16
    [    0.000000] arch_timer: cp15 timer(s) running at 24.00MHz (virt).
    [    0.000000] clocksource: arch_sys_counter: mask: 0xffffffffffffff max_cycles: 0x588fe9dc0, max_idle_ns: 440795202592 ns
    [    0.000008] sched_clock: 56 bits at 24MHz, resolution 41ns, wraps every 4398046511097ns
    [    0.000019] Switching to timer-based delay loop, resolution 41ns
    [    0.000187] clocksource: timer: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
    [    0.000420] Console: colour dummy device 80x30
    [    0.000457] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=240000)
    [    0.000475] pid_max: default: 32768 minimum: 301
    [    0.000604] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes)
    [    0.000619] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes)
    [    0.001213] CPU: Testing write buffer coherency: ok
    [    0.001589] /cpus/cpu@0 missing clock-frequency property
    [    0.001612] CPU0: thread -1, cpu 0, socket 0, mpidr 80000000
    [    0.002074] Setting up static identity map for 0x40100000 - 0x40100060
    [    0.002259] Hierarchical SRCU implementation.
    [    0.002765] smp: Bringing up secondary CPUs ---
    [    0.002781] smp: Brought up 1 node, 1 CPU
    [    0.002790] SMP: Total of 1 processors activated (48.00 BogoMIPS).
    [    0.002797] CPU: All CPU(s) started in SVC mode.
    [    0.003559] devtmpfs: initialized
    [    0.006668] VFP support v0.3: implementor 41 architecture 2 part 30 variant 7 rev 5
    [    0.006932] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
    [    0.006967] futex hash table entries: 256 (order: 2, 16384 bytes)
    [    0.007136] pinctrl core: initialized pinctrl subsystem
    [    0.008026] random: get_random_u32 called from bucket_table_alloc+0xf4/0x244 with crng_init=0
    [    0.008162] NET: Registered protocol family 16
    [    0.008655] DMA: preallocated 256 KiB pool for atomic coherent allocations
    [    0.009800] hw-breakpoint: found 5 (+1 reserved) breakpoint and 4 watchpoint registers.
    [    0.009817] hw-breakpoint: maximum watchpoint size is 8 bytes.
    [    0.023260] SCSI subsystem initialized
    [    0.023562] usbcore: registered new interface driver usbfs
    [    0.023652] usbcore: registered new interface driver hub
    [    0.023747] usbcore: registered new device driver usb
    [    0.023983] Linux video capture interface: v2.00
    [    0.024036] pps_core: LinuxPPS API ver. 1 registered
    [    0.024044] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
    [    0.024064] PTP clock support registered
    [    0.024282] Advanced Linux Sound Architecture Driver Initialized.
    [    0.024955] Bluetooth: Core ver 2.22
    [    0.025029] NET: Registered protocol family 31
    [    0.025037] Bluetooth: HCI device and connection manager initialized
    [    0.025056] Bluetooth: HCI socket layer initialized
    [    0.025066] Bluetooth: L2CAP socket layer initialized
    [    0.025097] Bluetooth: SCO socket layer initialized
    [    0.026313] clocksource: Switched to clocksource arch_sys_counter
    [    0.037157] NET: Registered protocol family 2
    [    0.037746] TCP established hash table entries: 1024 (order: 0, 4096 bytes)
    [    0.037780] TCP bind hash table entries: 1024 (order: 1, 8192 bytes)
    [    0.037803] TCP: Hash tables configured (established 1024 bind 1024)
    [    0.037937] UDP hash table entries: 256 (order: 1, 8192 bytes)
    [    0.037985] UDP-Lite hash table entries: 256 (order: 1, 8192 bytes)
    [    0.038205] NET: Registered protocol family 1
    [    0.038812] RPC: Registered named UNIX socket transport module.
    [    0.038833] RPC: Registered udp transport module.
    [    0.038840] RPC: Registered tcp transport module.
    [    0.038846] RPC: Registered tcp NFSv4.1 backchannel transport module.
    [    0.040940] workingset: timestamp_bits=30 max_order=14 bucket_order=0
    [    0.048568] NFS: Registering the id_resolver key type
    [    0.048618] Key type id_resolver registered
    [    0.048627] Key type id_legacy registered
    [    0.048672] jffs2: version 2.2. (NAND) © 2001-2006 Red Hat, Inc.
    [    0.050140] random: fast init done
    [    0.053091] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 249)
    [    0.053111] io scheduler noop registered
    [    0.053118] io scheduler deadline registered
    [    0.053358] io scheduler cfq registered (default)
    [    0.053368] io scheduler mq-deadline registered
    [    0.053376] io scheduler kyber registered
    [    0.057981] sun8i-v3s-pinctrl 1c20800.pinctrl: initialized sunXi PIO driver
    [    0.058417] name=allwinner,sun7i-a20-pwm
    [    0.058432] npwm=2
    [    0.127969] Serial: 8250/16550 driver, 8 ports, IRQ sharing disabled
    [    0.131445] console [ttyS0] disabled
    [    0.151721] 1c28000.serial: ttyS0 at MMIO 0x1c28000 (irq = 33, base_baud = 1500000) is a U6_16550A
    [    0.780269] console [ttyS0] enabled
    [    0.805297] 1c28400.serial: ttyS1 at MMIO 0x1c28400 (irq = 34, base_baud = 1500000) is a U6_16550A
    [    0.835807] 1c28800.serial: ttyS2 at MMIO 0x1c28800 (irq = 35, base_baud = 1500000) is a U6_16550A
    [    0.848508] libphy: Fixed MDIO Bus: probed
    [    0.853001] usbcore: registered new interface driver r8152
    [    0.858614] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
    [    0.865135] ehci-platform: EHCI generic platform driver
    [    0.870676] ehci-platform 1c1a000.usb: EHCI Host Controller
    [    0.876350] ehci-platform 1c1a000.usb: new USB bus registered, assigned bus number 1
    [    0.884288] ehci-platform 1c1a000.usb: irq 25, io mem 0x01c1a000
    [    0.916344] ehci-platform 1c1a000.usb: USB 2.0 started, EHCI 1.00
    [    0.923490] hub 1-0:1.0: USB hub found
    [    0.927421] hub 1-0:1.0: 1 port detected
    [    0.931878] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
    [    0.938171] ohci-platform: OHCI generic platform driver
    [    0.943713] ohci-platform 1c1a400.usb: Generic Platform OHCI controller
    [    0.950433] ohci-platform 1c1a400.usb: new USB bus registered, assigned bus number 2
    [    0.958360] ohci-platform 1c1a400.usb: irq 26, io mem 0x01c1a400
    [    1.031375] hub 2-0:1.0: USB hub found
    [    1.035198] hub 2-0:1.0: 1 port detected
    [    1.042745] udc-core: couldn't find an available UDC - added [g_ether] to list of pending drivers
    [    1.052618] sun6i-rtc 1c20400.rtc: rtc core: registered rtc-sun6i as rtc0
    [    1.059513] sun6i-rtc 1c20400.rtc: RTC enabled
    [    1.064048] i2c /dev entries driver
    [    1.069222] usbcore: registered new interface driver uvcvideo
    [    1.074974] USB Video Class driver (1.1.1)
    [    1.079833] sunxi-wdt 1c20ca0.watchdog: Watchdog enabled (timeout=16 sec, nowayout=0)
    [    1.087825] Bluetooth: HCI UART driver ver 2.3
    [    1.092273] Bluetooth: HCI UART protocol Three-wire (H5) registered
    [    1.156357] sunxi-mmc 1c0f000.mmc: base:0xc407b000 irq:23
    [    1.162805] usbcore: registered new interface driver usbhid
    [    1.168456] usbhid: USB HID core driver
    [    1.174122] NET: Registered protocol family 17
    [    1.178794] Key type dns_resolver registered
    [    1.183228] Registering SWP/SWPB emulation handler
    [    1.193806] simple-framebuffer 43e89000.framebuffer: framebuffer at 0x43e89000, 0x177000 bytes, mapped to 0xc4400000
    [    1.204454] simple-framebuffer 43e89000.framebuffer: format=x8r8g8b8, mode=800x480x32, linelength=3200
    [    1.222854] Console: switching to colour frame buffer device 100x30
    [    1.235317] simple-framebuffer 43e89000.framebuffer: fb0: simplefb registered!
    [    1.243916] usb_phy_generic usb_phy_generic.0.auto: usb_phy_generic.0.auto supply vcc not found, using dummy regulator
    [    1.255346] musb-hdrc musb-hdrc.1.auto: MUSB HDRC host driver
    [    1.261186] musb-hdrc musb-hdrc.1.auto: new USB bus registered, assigned bus number 3
    [    1.270315] hub 3-0:1.0: USB hub found
    [    1.274184] hub 3-0:1.0: 1 port detected
    [    1.279498] using random self ethernet address
    [    1.283985] using random host ethernet address
    [    1.289160] usb0: HOST MAC 8e:ca:5f:61:47:a8
    [    1.293475] usb0: MAC f2:57:f4:ad:74:af
    [    1.297416] using random self ethernet address
    [    1.301858] using random host ethernet address
    [    1.306400] g_ether gadget: Ethernet Gadget, version: Memorial Day 2008
    [    1.313010] g_ether gadget: g_ether ready
    [    1.317402] sun6i-rtc 1c20400.rtc: setting system clock to 1970-01-01 00:56:52 UTC (3412)
    [    1.325834] vcc3v0: disabling
    [    1.328911] vcc5v0: disabling
    [    1.331879] ALSA device list:
    [    1.334841]   No soundcards found.
    [    1.339241] VFS: Cannot open root device "31:03" or unknown-block(31,3): error -19
    [    1.346938] Please append a correct "root=" boot option; here are the available partitions:
    [    1.355286] Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(31,3)
    [    1.363630] CPU: 0 PID: 1 Comm: swapper/0 Not tainted 4.13.0-licheepi-zero+ #10
    [    1.370926] Hardware name: Allwinner sun8i Family
    [    1.375664] [<c010e58c>] (unwind_backtrace) from [<c010b2b0>] (show_stack+0x10/0x14)
    [    1.383408] [<c010b2b0>] (show_stack) from [<c06923dc>] (dump_stack+0x84/0x98)
    [    1.390632] [<c06923dc>] (dump_stack) from [<c011b728>] (panic+0xdc/0x248)
    [    1.397507] [<c011b728>] (panic) from [<c09011d0>] (mount_block_root+0x188/0x25c)
    [    1.404985] [<c09011d0>] (mount_block_root) from [<c09013c4>] (mount_root+0x120/0x128)
    [    1.412894] [<c09013c4>] (mount_root) from [<c090151c>] (prepare_namespace+0x150/0x194)
    [    1.420891] [<c090151c>] (prepare_namespace) from [<c0900e20>] (kernel_init_freeable+0x1bc/0x1cc)
    [    1.429755] [<c0900e20>] (kernel_init_freeable) from [<c06a514c>] (kernel_init+0x8/0x108)
    [    1.437927] [<c06a514c>] (kernel_init) from [<c0107638>] (ret_from_fork+0x14/0x3c)
    [    1.445499] Rebooting in 5 seconds..

    U-Boot SPL 2017.01-rc2-00073-gdd6e874-dirty (Nov 26 2017 - 15:10:41)
    DRAM: 64 MiB
    Trying to boot from sunxi SPICPU:   Allwinner V3s (SUN8I 1681)
    Model: Lichee Pi Zero
    DRAM:  64 MiB
    MMC:   SUNXI SD/MMC: 0
    SF: Detected w25q128bv with page size 256 Bytes, erase size 64 KiB, total 16 MiB
    *** Warning - bad CRC, using default environment

    Setting up a 800x480 lcd console (overscan 0x0)
    dotclock: 27000kHz = 27000kHz: (1 * 3MHz * 54) / 6
    beep 0
    beep 1
    beep 0
    beep 1
    beep 0
    beep 1
    In:    serial@01c28000
    Out:   serial@01c28000
    Err:   serial@01c28000
    CPU:   Allwinner V3s (SUN8I 1681)
    Model: Lichee Pi Zero
    DRAM:  64 MiB
    MMC:   SUNXI SD/MMC: 0
    SF: Detected w25q128bv with page size 256 Bytes, erase size 64 KiB, total 16 MiB
    *** Warning - bad CRC, using default environment

    Setting up a 800x480 lcd console (overscan 0x0)
    dotclock: 27000kHz = 27000kHz: (1 * 3MHz * 54) / 6
    beep 0
    beep 1
    beep 0
    beep 1
    beep 0
    beep 1
    In:    serial@01c28000
    Out:   serial@01c28000
    Err:   serial@01c28000
    Net:   No ethernet found.
    starting USB---
    No controllers found
    Hit any key to stop autoboot:  0 
    SF: Detected w25q128bv with page size 256 Bytes, erase size 64 KiB, total 16 MiB
    device 0 offset 0x100000, size 0x10000
    SF: 65536 bytes @ 0x100000 Read: OK
    device 0 offset 0x110000, size 0x400000
    SF: 4194304 bytes @ 0x110000 Read: OK
    ## Flattened Device Tree blob at 41800000
    Booting using the fdt blob at 0x41800000
    Loading Device Tree to 42dfa000, end 42dffbfa --- OK

    Starting kernel ---

    [    0.000000] Booting Linux on physical CPU 0x0
    [    0.000000] Linux version 4.13.0-licheepi-zero+ (root@bf756b445919) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05)) #95 SMP Mon Nov 27 01:20:31 UTC 2017
    [    0.000000] CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=10c5387d
    [    0.000000] CPU: div instructions available: patching division code
    [    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
    [    0.000000] OF: fdt: Machine model: Lichee Pi Zero XFJ
    [    0.000000] Memory policy: Data cache writealloc
    [    0.000000] percpu: Embedded 15 pages/cpu @c3de7000 s32588 r8192 d20660 u61440
    [    0.000000] Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 15883
    [    0.000000] Kernel command line: console=ttyS0,115200 earlyprintk panic=5 rootwait mtdparts=spi32766.0:1M(uboot)ro,64k(dtb)ro,4M(kernel)ro,-(rootfs) root=31:03 rw rootfstype=jffs2
    [    0.000000] PID hash table entries: 256 (order: -2, 1024 bytes)
    [    0.000000] Dentry cache hash table entries: 8192 (order: 3, 32768 bytes)
    [    0.000000] Inode-cache hash table entries: 4096 (order: 2, 16384 bytes)
    [    0.000000] Memory: 55708K/64036K available (4096K kernel code, 187K rwdata, 1144K rodata, 1024K init, 232K bss, 8328K reserved, 0K cma-reserved, 0K highmem)
    [    0.000000] Virtual kernel memory layout:
    [    0.000000]     vector  : 0xffff0000 - 0xffff1000   (   4 kB)
    [    0.000000]     fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
    [    0.000000]     vmalloc : 0xc4000000 - 0xff800000   ( 952 MB)
    [    0.000000]     lowmem  : 0xc0000000 - 0xc3e89000   (  62 MB)
    [    0.000000]     pkmap   : 0xbfe00000 - 0xc0000000   (   2 MB)
    [    0.000000]     modules : 0xbf000000 - 0xbfe00000   (  14 MB)
    [    0.000000]       .text : 0xc0008000 - 0xc0500000   (5088 kB)
    [    0.000000]       .init : 0xc0700000 - 0xc0800000   (1024 kB)
    [    0.000000]       .data : 0xc0800000 - 0xc082ee00   ( 188 kB)
    [    0.000000]        .bss : 0xc08332d0 - 0xc086d584   ( 233 kB)
    [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
    [    0.000000] Hierarchical RCU implementation.
    [    0.000000]  RCU event tracing is enabled.
    [    0.000000]  RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=1.
    [    0.000000] RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=1
    [    0.000000] NR_IRQS: 16, nr_irqs: 16, preallocated irqs: 16
    [    0.000000] arch_timer: cp15 timer(s) running at 24.00MHz (virt).
    [    0.000000] clocksource: arch_sys_counter: mask: 0xffffffffffffff max_cycles: 0x588fe9dc0, max_idle_ns: 440795202592 ns
    [    0.000009] sched_clock: 56 bits at 24MHz, resolution 41ns, wraps every 4398046511097ns
    [    0.000024] Switching to timer-based delay loop, resolution 41ns
    [    0.000214] clocksource: timer: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
    [    0.000465] Console: colour dummy device 80x30
    [    0.000506] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=240000)
    [    0.000525] pid_max: default: 32768 minimum: 301
    [    0.000672] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes)
    [    0.000692] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes)
    [    0.001393] CPU: Testing write buffer coherency: ok
    [    0.001817] /cpus/cpu@0 missing clock-frequency property
    [    0.001842] CPU0: thread -1, cpu 0, socket 0, mpidr 80000000
    [    0.002345] Setting up static identity map for 0x40100000 - 0x40100060
    [    0.002541] Hierarchical SRCU implementation.
    [    0.003148] smp: Bringing up secondary CPUs ---
    [    0.003163] smp: Brought up 1 node, 1 CPU
    [    0.003175] SMP: Total of 1 processors activated (48.00 BogoMIPS).
    [    0.003184] CPU: All CPU(s) started in SVC mode.
    [    0.004066] devtmpfs: initialized
    [    0.007259] VFP support v0.3: implementor 41 architecture 2 part 30 variant 7 rev 5
    [    0.007578] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
    [    0.007614] futex hash table entries: 256 (order: 2, 16384 bytes)
    [    0.007813] pinctrl core: initialized pinctrl subsystem
    [    0.008896] DMA: preallocated 256 KiB pool for atomic coherent allocations
    [    0.010158] hw-breakpoint: found 5 (+1 reserved) breakpoint and 4 watchpoint registers.
    [    0.010179] hw-breakpoint: maximum watchpoint size is 8 bytes.
    [    0.022957] SCSI subsystem initialized
    [    0.023159] usbcore: registered new interface driver usbfs
    [    0.023235] usbcore: registered new interface driver hub
    [    0.023362] usbcore: registered new device driver usb
    [    0.023474] Linux video capture interface: v2.00
    [    0.023517] pps_core: LinuxPPS API ver. 1 registered
    [    0.023526] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
    [    0.023695] Advanced Linux Sound Architecture Driver Initialized.
    [    0.024215] clocksource: Switched to clocksource arch_sys_counter
    [    0.039085] workingset: timestamp_bits=30 max_order=14 bucket_order=0
    [    0.046435] squashfs: version 4.0 (2009/01/31) Phillip Lougher
    [    0.046808] jffs2: version 2.2. (NAND) ? 2001-2006 Red Hat, Inc.
    [    0.048756] random: fast init done
    [    0.052407] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 250)
    [    0.052432] io scheduler noop registered
    [    0.052442] io scheduler deadline registered
    [    0.052669] io scheduler cfq registered (default)
    [    0.052681] io scheduler mq-deadline registered
    [    0.052690] io scheduler kyber registered
    [    0.057653] sun8i-v3s-pinctrl 1c20800.pinctrl: initialized sunXi PIO driver
    [    0.058150] name=allwinner,sun7i-a20-pwm
    [    0.058168] npwm=2
    [    0.139525] Serial: 8250/16550 driver, 8 ports, IRQ sharing disabled
    [    0.143224] console [ttyS0] disabled
    [    0.163532] 1c28000.serial: ttyS0 at MMIO 0x1c28000 (irq = 33, base_baud = 1500000) is a U6_16550A
    [    0.676919] console [ttyS0] enabled
    [    0.702093] 1c28400.serial: ttyS1 at MMIO 0x1c28400 (irq = 34, base_baud = 1500000) is a U6_16550A
    [    0.732771] 1c28800.serial: ttyS2 at MMIO 0x1c28800 (irq = 35, base_baud = 1500000) is a U6_16550A
    [    0.747219] m25p80 spi32766.0: w25q128 (16384 Kbytes)
    [    0.752288] in cmdline partion
    [    0.755486] p4 : size=100000
    [    0.758369] p4 : size=10000
    [    0.761160] p4 : size=400000
    [    0.764037] p4 : size=ffffffff
    [    0.767126] spi32766.0: parser cmdlinepart: 4
    [    0.771481] 4 cmdlinepart partitions found on MTD device spi32766.0
    [    0.777758] Creating 4 MTD partitions on "spi32766.0":
    [    0.782904] 0x000000000000-0x000000100000 : "uboot"
    [    0.789535] 0x000000100000-0x000000110000 : "dtb"
    [    0.795874] 0x000000110000-0x000000510000 : "kernel"
    [    0.802292] 0x000000510000-0x000001000000 : "rootfs"
    [    0.809706] sun6i-rtc 1c20400.rtc: rtc core: registered rtc-sun6i as rtc0
    [    0.816602] sun6i-rtc 1c20400.rtc: RTC enabled
    [    0.821153] i2c /dev entries driver
    [    0.826715] sunxi-wdt 1c20ca0.watchdog: Watchdog enabled (timeout=16 sec, nowayout=0)
    [    0.835775] usbcore: registered new interface driver usbhid
    [    0.841355] usbhid: USB HID core driver
    [    0.846926] Registering SWP/SWPB emulation handler
    [    0.856875] simple-framebuffer 43e89000.framebuffer: framebuffer at 0x43e89000, 0x177000 bytes, mapped to 0xc4400000
    [    0.867547] simple-framebuffer 43e89000.framebuffer: format=x8r8g8b8, mode=800x480x32, linelength=3200
    [    0.885830] Console: switching to colour frame buffer device 100x30
    [    0.900736] simple-framebuffer 43e89000.framebuffer: fb0: simplefb registered!
    [    0.908121] sun6i-rtc 1c20400.rtc: setting system clock to 1970-01-01 02:33:59 UTC (9239)
    [    0.916575] vcc3v0: disabling
    [    0.919554] vcc3v3: disabling
    [    0.922519] vcc5v0: disabling
    [    0.925539] ALSA device list:
    [    0.928507]   No soundcards found.
    [    0.994326] random: crng init done
    [    1.519199] VFS: Mounted root (jffs2 filesystem) on device 31:3.
    [    1.526365] devtmpfs: mounted
    [    1.530825] Freeing unused kernel memory: 1024K
    Starting logging: OK
    Starting mdev---
    modprobe: can't change directory to '/lib/modules': No such file or directory
    Initializing random number generator--- done.
    Starting network: ip: socket: Function not implemented
    ip: socket: Function not implemented
    FAIL

    Welcome to Lichee Pi
    Lichee login: 

## 附录：sunxi-fel帮助说明


```
sunxi-fel v1.4.1-87-g78a7566

Usage: sunxi-fel [options] command arguments--- [command---]
    -h, --help          Print this usage summary and exit
    -v, --verbose           Verbose logging
    -p, --progress          "write" transfers show a progress bar
    -l, --list          Enumerate all (USB) FEL devices and exit
    -d, --dev bus:devnum        Use specific USB bus and device number
        --sid SID           Select device by SID key (exact match)

    spl file            Load and execute U-Boot SPL
        If file additionally contains a main U-Boot binary
        (u-boot-sunxi-with-spl.bin), this command also transfers that
        to memory (default address from image), but won't execute it.

    uboot file-with-spl     like "spl", but actually starts U-Boot
        U-Boot execution will take place when the fel utility exits.
        This allows combining "uboot" with further "write" commands
        (to transfer other files needed for the boot).

    hex[dump] address length    Dumps memory region in hex
    dump address length     Binary memory dump
    exe[cute] address       Call function address
    reset64 address         RMR request for AArch64 warm boot
    memmove dest source size    Copy <size> bytes within device memory
    readl address           Read 32-bit value from device memory
    writel address value        Write 32-bit value to device memory
    read address length file    Write memory contents into file
    write address file      Store file contents into memory
    write-with-progress addr file   "write" with progress bar
    write-with-gauge addr file  Output progress for "dialog --gauge"
    write-with-xgauge addr file Extended gauge output (updates prompt)
    multi[write] # addr file ---    "write-with-progress" multiple files,
                    sharing a common progress status
    multi[write]-with-gauge --- like their "write-with-*" counterpart,
    multi[write]-with-xgauge ---      but following the 'multi' syntax:
                    <#> addr file [addr file [---]]
    echo-gauge "some text"      Update prompt/caption for gauge output
    ver[sion]           Show BROM version
    sid             Retrieve and output 128-bit SID key
    clear address length        Clear memory
    fill address length value   Fill memory
    spiflash-info           Retrieves basic information
    spiflash-hex[dump] addr length  Dumps SPI flash region in hex
    spiflash-read addr length file  Write SPI flash contents into file
    spiflash-write addr file    Store file contents into SPI flash
```

## 参考资料
<a href="http://blog.sina.com.cn/s/blog_5ed5a1f40100f3qq.html" target="_black">http://blog.sina.com.cn/s/blog_5ed5a1f40100f3qq.html</a>
