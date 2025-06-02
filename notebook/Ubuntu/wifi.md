# WiFi

## [nmcli](https://www.cnblogs.com/mind-water/p/12079647.html) 

```bash
# 启用WIFI
$ nmcli radio wifi on

# 查询WIFI状态
$ nmcli dev status

# 搜索WIFI
$ nmcli dev wifi list

# 连接WIFI
$ sudo nmcli dev wifi connect "ssid" password "pwd"

# 查询连接的网络
$ nmcli connection show

# 断开网络连接
$ nmcli con down ssid / uuid 　　

# 连接另外一网络
$ nmcli con up ssid / uuid 

# 关闭WIFI
$ nmcli radio wifi off
```

## example

```bash
# 启用WIFI
$ nmcli radio wifi on

# 查询WIFI状态
$ nmcli dev status
DEVICE         TYPE      STATE         CONNECTION
wlan0          wifi      connected     HUAWEI-Y6AZGD
eth0           ethernet  disconnected  --
lo             loopback  unmanaged     --
p2p-dev-wlan0  wifi-p2p  unmanaged     --

# 搜索WIFI
$ nmcli dev wifi list
[dhd-wlan0] wl_run_escan : LEGACY_SCAN sync ID: 1, bssidx: 0
IN-USE  BSSID              SSID            MODE   CHAN  RATE        SIGNAL  BAR>
        34:71:46:E3:87:9C  HUAWEI-10GA2K   Infra  6     130 Mbit/s  100     ▂▄▆>
*       44:59:E3:CB:89:F4  HUAWEI-Y6AZGD   Infra  1     130 Mbit/s  92      ▂▄▆>
        44:59:E3:CB:89:F5  --              Infra  1     130 Mbit/s  90      ▂▄▆>
        44:59:E3:CB:89:F9  --              Infra  1     130 Mbit/s  85      ▂▄▆>
        DC:C6:4B:C6:73:44  ChinaNet-VC99   Infra  6     130 Mbit/s  57      ▂▄▆>
        30:B4:9E:1D:AA:6A  TP-LINK_1DAA6A  Infra  1     270 Mbit/s  47      ▂▄_>
        F4:84:8D:CA:35:C0  Topway_214370   Infra  11    270 Mbit/s  45      ▂▄_>
        FE:84:8D:CA:35:C0  --              Infra  11    270 Mbit/s  45      ▂▄_>
        
# 连接WIFI
$ sudo nmcli dev wifi connect "HUAWEI-Y6AZGD" password "12345678"
Device 'wlan0' successfully activated with '1a58b126-89c3-4e5f-9a1b-7bfdbea1c666'.

# 查询连接的网络
$ nmcli connection show
NAME                UUID                                  TYPE      DEVICE
Wired connection 1  c7cdffc8-5630-3c5e-96a6-aa768522d982  ethernet  eth0
HUAWEI-Y6AZGD       1a58b126-89c3-4e5f-9a1b-7bfdbea1c666  wifi      wlan0
HUAWEI-10GA2K       f19f6baa-5d68-4366-8002-69fc64905994  wifi      --

# 查看网络状态
$ ifconfig
eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet6 fe80::121b:b91d:7e06:57  prefixlen 64  scopeid 0x20<link>
        ether de:82:66:c4:6e:2c  txqueuelen 1000  (Ethernet)
        RX packets 2126  bytes 354282 (354.2 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4903  bytes 772837 (772.8 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device interrupt 37

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 90041  bytes 32762063 (32.7 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 90041  bytes 32762063 (32.7 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

wlan0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.3.236  netmask 255.255.255.0  broadcast 192.168.3.255
        inet6 240e:3b7:324f:29d0:e995:16f0:4972:74aa  prefixlen 64  scopeid 0x0<global>
        inet6 240e:3b7:324f:29d0:4459:e3cb:89ee:6  prefixlen 128  scopeid 0x0<global>
        inet6 240e:3b7:324f:29d0:14e:71ee:ffce:4047  prefixlen 64  scopeid 0x0<global>
        inet6 fe80::6575:3a1d:5af2:8b35  prefixlen 64  scopeid 0x20<link>
        ether c0:f5:35:fa:e0:e5  txqueuelen 1000  (Ethernet)
        RX packets 57193  bytes 7228692 (7.2 MB)
        RX errors 0  dropped 2  overruns 0  frame 0
        TX packets 60362  bytes 28884325 (28.8 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

# ping 测试
$ ping baidu.com
PING baidu.com (39.156.66.10): 56 data bytes
64 bytes from 39.156.66.10: icmp_seq=0 ttl=49 time=130.730 ms
64 bytes from 39.156.66.10: icmp_seq=1 ttl=49 time=57.464 ms
64 bytes from 39.156.66.10: icmp_seq=2 ttl=49 time=50.247 ms
64 bytes from 39.156.66.10: icmp_seq=3 ttl=49 time=44.222 ms
64 bytes from 39.156.66.10: icmp_seq=4 ttl=49 time=51.131 ms
64 bytes from 39.156.66.10: icmp_seq=5 ttl=49 time=56.861 ms
64 bytes from 39.156.66.10: icmp_seq=6 ttl=49 time=54.452 ms
64 bytes from 39.156.66.10: icmp_seq=7 ttl=49 time=56.723 ms
64 bytes from 39.156.66.10: icmp_seq=8 ttl=49 time=46.005 ms
```



