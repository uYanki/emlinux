#### driver

##### install

将驱动源码交叉编译为内核模块 `.ko` 文件，拷贝到目标机，通过 `insmod` 指令安装：

```
insmod 8188eu.ko
```

#### wifi

##### config

```
cd /etc
vi wpa_supplicant.conf
```

add the following content

```shell
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
ap_scan=1
network={
    ssid="HUAWEI-Y6AZGD"
    scan_ssid=1
    key_mgmt=WPA-EAP WPA-PSK IEEE8021X NONE
    pairwise=TKIP CCMP
    group=CCMP TKIP WEP104 WEP40
    psk="password"
    priority=5
}
```

##### init

根据配置文件来初始化 wlan0

```
wpa_supplicant -B -c wpa_supplicant.conf -i wlan0
```

##### scan

```shell
wpa_cli -i wlan0 scan 
wpa_cli -i wlan0 scan_result # log info
```

##### get ip

```
chmod 0755 /usr/share/udhcpc/default.script 
udhcpc -i wlan0
```



