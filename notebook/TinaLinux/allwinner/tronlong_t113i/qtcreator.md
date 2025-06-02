# QT Creator

[参考文档](https://doc.embedfire.com/linux/rk356x/Qt/zh/latest/lubancat_qt/install/remote_connect.html) 

# 环境搭建

```bash
# 安装 qtcreator
$ apt-get install qtcreator
```

## 构建和运行

### Debugger

![image-20250602163849332](.assets/qtcreator/image-20250602163849332.png)

### 编译器

![image-20250602164007622](.assets/qtcreator/image-20250602164007622.png)

![image-20250602164026509](.assets/qtcreator/image-20250602164026509.png)

### QMake

![image-20250602164048093](.assets/qtcreator/image-20250602164048093.png)

### CMake

![image-20250602164112669](.assets/qtcreator/image-20250602164112669.png)

### 构建套件 Kit

![image-20250602164144606](.assets/qtcreator/image-20250602164144606.png)

### 远程设备

![image-20250602164231620](.assets/qtcreator/image-20250602164231620.png)

![image-20250602164319531](.assets/qtcreator/image-20250602164319531.png)

地址为 SSH 地址。点击 Test 测试连接是否正常。

# 远程部署

## SFTP 上传

在 .pro 文件添加

```c
target.path = /root
INSTALLS += target
```

![image-20250602163554809](.assets/qtcreator/image-20250602163554809.png)

## 可执行文件

```
-platform linuxfb
```

![image-20250602163444762](.assets/qtcreator/image-20250602163444762.png)

## 环境变量

```
LD_LIBRARY_PATH=/usr/local/Qt_5.12.5/lib
QT_QPA_FONTDIR=/usr/local/Qt_5.12.5/fonts
QT_QPA_PLATFORM=linuxfb
QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/Qt_5.12.5/plugins
QT_ROOT=/usr/local/Qt_5.12.5
```

![image-20250602163300329](.assets/qtcreator/image-20250602163300329.png)

![image-20250602163324841](.assets/qtcreator/image-20250602163324841.png)



