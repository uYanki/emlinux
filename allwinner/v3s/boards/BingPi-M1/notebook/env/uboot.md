# U-Boot

1. 进入 u-boot 目录

```bash
cd /home/zh/v3s/u-boot/uboot_v3s-master/u-boot
```

> 不要使用 u-boot-2022.04 及以上版本，可能报提示编译器版本过高等错误。

2. 屏幕配置

不同屏幕不同配置（在 configs 文件夹下）

```bash
# 默认
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LicheePi_Zero_defconfig

# 4.3 普清屏
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LicheePi_Zero_480x272LCD_defconfig

# 4.3 高清屏（7.0 普清） 
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LicheePi_Zero_800x480LCD_defconfig
```

3. TF 卡启动

```bash
vim include/configs/sun8i.h
```

![image-20250608161659473](.assets/uboot/image-20250608161659473.png)

4. 配置

```bash
make ARCH=arm menuconfig
```

5. 编译

```bash
ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make -j4
```

![image-20250608162202487](.assets/uboot/image-20250608162202487.png)


6. 烧录

```bash
sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/sdb bs=1024 seek=8
```

