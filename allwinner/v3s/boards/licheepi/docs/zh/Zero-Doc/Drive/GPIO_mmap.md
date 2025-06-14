---
title: C语言方式(mmap)
---

这里提供接近单片机寄存器操作的一种应用层GPIO操作方式，也封装成库给大家使用。

## GPIO 寄存器介绍


在 [V3S datasheet](http://lichee.jicm.cn/doc/V3S/Allwinner_V3s_Datasheet_V1.0.pdf) 第224页是GPIO控制器的相关介绍。

V3S从有PB/C/E/F/G五个GPIO端口，每个都是32位端口（实际引脚没有引出那么多）,也是32位寄存器。

每个端口由以下几个寄存器组成：

> (n=1,2,4,5,6；寄存器基址为0x01C20800)

| Register Name | Offset | Description | 详细描述 |
|---- |----- | ------ | ------ |
| Pn_CFG0 | n*0x24+0x00 | Port n Configure Register 0 (n=1,2,4,5,6) | 每个脚4bit，最高位保留。000 输入;001 输出;010 外设功能1;011 外设功能2;100 外设功能3;101 外设功能4;110 EINT中断;111 IO失能|
| Pn_CFG1 | n*0x24+0x04 | Port n Configure Register 1 | 同上 |
| Pn_CFG2 | n*0x24+0x08 | Port n Configure Register 2 | 同上 |
| Pn_CFG3 | n*0x24+0x0C | Port n Configure Register 3 | 同上 |
| Pn_DAT | n*0x24+0x10 | Port n Data Register | 每位代表输入输出值 |
| Pn_DRV0 | n*0x24+0x14 | Port n Multi-Driving Register 0 | 0~3逐级递增 |
| Pn_DRV1 | n*0x24+0x18 | Port n Multi-Driving Register 1 | 同上 |
| Pn_PUL0 | n*0x24+0x1C | Port n PullRegister 0 | 0浮空，1上拉，2下拉，3保留 |
| Pn_PUL1 | n*0x24+0x20 | Port n PullRegister 1  | 同上 |
| Pn_INT_CFG0 | 0x200+n*0x20+0x00 | PIO Interrrupt Configure Register 0 | 0上升，1下降，2高电平，3低电平，4双边沿 |
| Pn_INT_CFG1 | 0x200+n*0x20+0x04 | PIO Interrrupt Configure Register 1 | 同上 |
| Pn_INT_CFG2 | 0x200+n*0x20+0x08 | PIO Interrrupt Configure Register 2 | 同上 |
| Pn_INT_CFG3 | 0x200+n*0x20+0x0C | PIO Interrrupt Configure Register 3 | 同上 |
| Pn_INT_CTL   |0x200+n*0x20+0x10|PIO Interrupt Control Register           |0失能，1使能 |
| Pn_INT_STA   |0x200+n*0x20+0x14|PIO Interrupt Status Register            |0未发生中断，1发生中断。写1清除  |
| Pn_INT_DEB   |0x200+n*0x20+0x18|PIO Interrupt Debounce Register  |bit0，选择中断时钟，0,32Khz，低速时钟；1，24MHz主时钟。bit6:4，去抖时钟分频，选择的时钟源2^n分频，即最大256分频。|

| 寄存器 | 地址  |
| ---- | ---- |
|PB配置|1C20824|
|PC配置|1C20848|
|PE配置|1C20890|
|PF配置|1C208B4|
|PG配置|1C208D8|

## mmap简介

mmap简单来说就是把一片物理内存空间（或者文件）映射到应用的虚拟内存空间，这样，直接在应用层就能操作CPU的寄存器，类似于单片机的寄存器操作。我们只要封装好寄存器操作的库函数，就能在以后的程序里简单调用了\~

详细的mmap介绍可以参考附录的链接。

为了操作寄存器，我们需要用到 */dev/mem* 设备，这个设备是是物理内存的全映像，可以用来访问物理内存，一般用法是 `open("/dev/mem",O_RDWR|O_SYNC)`，然后mmap，接着就可以用mmap的地址来访问物理内存，这实际上就是实现用户空间驱动的一种方法。

```
#include <sys/mmap.h>
void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
//start: 映射区的起始地址，0的话由接口自动返回
//length: 映射区的长度
//prot: 内存保护标志，不能与文件的打开模式冲突。以下值可以 或 组合。
    //PROT_EXEC  页内容可以被执行
    //PROT_READ  页内容可以被读取
    //PROT_WRITE  页可以被写入
    //PROT_NONE  页不可访问
//flag: 指定映射对象的类型，是否可以共享等。
//fd: 文件描述符
//oft: 被映射对象内容的起点偏移。映射物理内存的话，就是物理内存地址。**必须页对齐。**

int munmap(void *start, size_t length);
//start: 前面获得的地址
//length: 映射区的大小。

int msync ( void * addr , size_t len, int flags) 
//一般说来，进程在映射空间的对共享内容的改变并不直接写回到磁盘文件中，往往在调用munmap（）后才执行该操作。可以通过调用msync()实现磁盘上文件内容与共享内存区的内容一致。 
//但是对于映射物理内存来说是直接作用的。
```

代码片段：

```
#include <sys/mmap.h>

char dev_name[] = "/dev/mem"; 
GPIO_REGISTER  *gpio_base; 
fd  =  open(dev_name,O_RDWR); 
if(fd<0){ 
    printf("open %s is error\n",dev_name); 
    return -1 ; 
} 
gpio_base = (GPIO_REGISTER *)mmap( 0, 0x32, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0x40060000 );
if(gpio_base == NULL){ 
    printf("gpio base mmap is error\n"); 
    close(fd); 
    return -1; 
} 
//后面就是对寄存器操作了
//结束后解除映射
munmap(gpio_base, 0x32);
```

我基于mmap写了个应用层调试寄存器的小程序，reg-dbger,在github上可以下载使用。

使用方法为：

    reg-dbger r reg_addr                        //读寄存器
    reg-dbger rb reg_addr bit_oft bit_cnt       //读寄存器的bit_oft开始的bit_cnt位
    reg-dbger w reg_addr value                  //写寄存器
    reg-dbger wb reg_addr bit_oft bit_cnt value //写寄存器的bit_oft开始的bit_cnt位
    reg-dbger dump reg_addr cnt                 //批量dump出cnt个寄存器值

比如操作gpio寄存器，点亮熄灭Zero上的绿色LED：

    # PG0
    # 配置寄存器 0x01C20800+6*0x24+0=1C208D8
    # 数据寄存器 0x01C20800+6*0x24+0x10 = 1C208E8
    reg-dbger r 1C208D8 
    reg-dbger r 1C208E8
    reg-dbger wb 1C208D8 0 3 1    #输出状态
    reg-dbger wb 1C208E8 0 1 0     #输出0，点亮

同样基于mmap写了个应用层操作GPIO的小程序，lpi-gpio,在github上可以下载使用。

使用方法为：

    lpi-gpio set PG0 out/in 0/1/2   //设置为输出的话,0低电平，1,2高电平；设置为输入，0下拉，1上拉，2浮空。
    lpi-gpio r PG0 
    lpi-gpio w PG0 0/1
    lpi-gpio pwm PG0 100 200    //PG0 pwm输出，两个参数分别表示高低电平的微秒数（>60us）
    lpi-gpio test PG0   //测试PG0用函数翻转IO的最大速率，结果为1.85MHz
    lpi-gpio tfast PG0  //测试PG0用软件翻转IO的最大速率，结果为10MHz

为方便在C语言里调用，我生成了gpio操作的动态库 [libgpio.so](https://libgpio.so/)，大家可以在c程序中调用。

这里是一个简单的使用例程：

```
#include "lpi_gpio.h"
#define USLEEP_T 61

int main()
{
        lpi_gpio_initlib();
        lpi_gpio_init(6, 0, 1, 0);
        while(1)
        {   //generate 1KHz PWM
                lpi_gpio_w(6, 0, 1);
                usleep(500-USLEEP_T);
                lpi_gpio_w(6, 0, 0);
                usleep(500-USLEEP_T);
        }
        lpi_gpio_deinitlib();
        return;
}
```

    //gcc -fPIC -shared -o libgpio.so lib_gpio.c    //编译生成动态库
    gcc test_gpio.c -L. -lgpio -o test_gpio     //编译生成应用程序
    LD_LIBRARY_PATH=. ./test_gpio   //运行应用程序，手工指定动态库位置
    //or add libgpio.so to /etc/ld.so.conf, ldconfig    

## 附录


mmap参考资料：<http://blog.chinaunix.net/uid-26669729-id-3077015.html>

linux动态库：<http://www.cnblogs.com/jiqingwu/p/linux_dynamic_lib_create.html>

linux静态库：<http://www.cnblogs.com/jiqingwu/p/4325382.html>
