```bash
# board config
make ARCH=arm licheepi_zero_defconfig 

# menuconfig
make ARCH=arm menuconfig 

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16 

# build modules
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16 INSTALL_MOD_PATH=out modules

# build modules (.ko)
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_install

# build dtbs
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs


make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage
```

