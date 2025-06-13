# RTC

```bash
# 设置系统时间
$ date -s 11:22:33
Thu Jan  1 11:22:33 UTC 1970

# 将系统时间写入RTC时间
$ hwclock -w -f /dev/rtc0

# 读取RTC时间
$ hwclock -r -f /dev/rtc0
Thu Jan  1 11:22:33 1970  0.000000 seconds
```

