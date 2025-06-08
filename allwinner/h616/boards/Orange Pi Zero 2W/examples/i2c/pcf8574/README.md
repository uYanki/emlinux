# LCD1602

```bash
su

# install library
cd ./lib
make install

# build then run demo
cd ../demo
make
i2cdetect -y 2
./app -a 0x27
```

