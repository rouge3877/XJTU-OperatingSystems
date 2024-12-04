ifneq ($(KERNELRELEASE),)
	obj-m := globalvar.o  # obj-m 用于指定编译外部内核模块
else
	KERNELDIR := /lib/modules/$(shell uname -r)/build  # 获取内核源代码目录
	PWD := $(shell pwd)  # 当前路径

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules  # 通过内核的构建系统编译模块
endif

