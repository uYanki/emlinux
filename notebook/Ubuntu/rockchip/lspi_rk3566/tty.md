# 串口收发 tty

## 收发测试

```bash
# 列举 tty 设备
$ ls /dev/ttyS*
/dev/ttyS1  /dev/ttyS3

# 打开设备 (115200 8N1)
$ stty -F /dev/ttyS3 115200

# 发送数据
$ echo "I am tspi of rk3566.\n" > /dev/ttyS3

# 接收数据
$ cat /dev/ttyS3
```

## 后台接收模式

```bash
# 接收数据(后台模式)
$ cat /dev/ttyS3 &
[1] 14212

# 杀死进程
$ ps
    PID TTY          TIME CMD
    432 ttyFIQ0  00:00:00 login
    619 ttyFIQ0  00:00:00 bash
  14212 ttyFIQ0  00:00:00 cat
  14380 ttyFIQ0  00:00:00 ps
$ kill 14212
```

