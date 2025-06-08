# **使用LVGL**

### 1. 下载lvgl源码

　　在工作目录下建立lvgl_demo，用于测试。
　　切换到lvgl_demo目录，分别下载lvgl和lvgl_driver，并切换到当前最新版本（v8.3）：

```
cd lvgl_demo
git clone https://github.com/lvgl/lvgl.git
cd lvgl
git checkout remotes/origin/release/v8.3
cd lvgl_demo
git clone https://github.com/lvgl/lv_drivers.git
cd lv_drivers
git checkout remotes/origin/release/v8.3
```

### 2. 修改配置文件

　　下载下面的压缩包，解压后可获得4个文件：lvgl_conf.h、lvgl_drv_conf.h、main.c和Makefile文件，并复制到lvgl_demo下。其中Makefile里的TOOL_PATH按照你的实际路径进行修改。
　　lvgl文件下载：[下载地址](http://doc.liulianpi.cn/files/lvgl_files.zip)
　　最终目录结构如下：
![img](.assets/%E4%BD%BF%E7%94%A8LVGL/index.php)
　　最后在lvgl_demo下运行make即可。