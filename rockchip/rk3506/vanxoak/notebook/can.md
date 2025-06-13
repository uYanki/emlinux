# sysfs

```bash
$ ls /sys/class/net/
can0   eth0   lo     wlan0      
```

# CAN

## 模式配置

### CAN

```bash
# 关闭CAN
$ ip link set can0 down

# 设置波特率
$ ip link set can0 type can bitrate 125000

# 启动CAN
$ ip link set can0 up
```

### CAN FD

```bash
# 关闭CAN
$ ip link set can0 down

# 设置总线的bus-off复位时间
$ ip link set can0 type can restart-ms 100

# 设置仲裁段500K波特率，数据段2M波特率
$ ip link set can0 up type can bitrate 500000 dbitrate 2000000 fd on

# 启动CAN
$ ip link set can0 up

# 查看CAN总线的状态
$ ip -d -s link show can0
```

> 复位时间 bus-off是指总线出现异常导致挂起后总线自动恢复的延时时间，
>
> 若不设置，系统默认会挂起总线等待用户手动重启总线。

## 收发测试

```bash
# 发送数据帧
$ cansend can0 123#11.22.33.44.55.66.77.88

# 接收模式
$ candump can0
can0  123   [8]  11 22 33 44 55 66 77 88
```

