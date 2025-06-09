ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

PWD := $(shell pwd)
KERNEL := /lib/modules/4.14.14-licheepi-zero/build

MODULE_NAME := hello_drv
TEST_APP := hello_app

# 内核模块目标
obj-m += $(MODULE_NAME).o

# 用户空间程序目标
SRCS := hello_app.c
OBJS := $(SRCS:.c=.o)

# 默认目标
all: module test

# 编译内核模块
module:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(PWD) modules

# 编译用户空间测试程序
test: $(OBJS)
	$(CROSS_COMPILE)gcc -o $(TEST_APP) $(OBJS)

# 编译用户空间对象文件
%.o: %.c
	$(CROSS_COMPILE)gcc -c -o $@ $<

# 清理规则
clean:
	$(MAKE) -C $(KERNEL) M=$(PWD) clean
	rm -f $(TEST_APP) $(OBJS)
