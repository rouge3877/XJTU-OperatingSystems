# obj-m += modify_syscall.o

# create: modify_syscall.c modify_new_syscall.c modify_old_syscall.c
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
# 	gcc modify_new_syscall.c -o modify_new_syscall
# 	gcc modify_old_syscall.c -o modify_old_syscall

# install: modify_syscall.ko
# 	@sudo insmod modify_syscall.ko
# 	@echo "..."
# 	@sudo dmesg | tail -n 10 | grep modify_syscall

# clean: 
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
# 	rm modify_new_syscall
# 	rm modify_old_syscall

# uninstall:
# 	@sudo rmmod -f modify_syscall
# 	@echo "..."
# 	@sudo dmesg | tail -n 10 | grep modify_syscall

# .PHONY: show create install clean uninstall


ifneq ($(KERNELRELEASE),)
	obj-m := modify_syscall.o       #obj-m指编译成外部模块
else
	KERNELDIR := /lib/modules/$(shell uname -r)/build  #定义一个变量，指向内核目录
	PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules  #编译内核模块
endif
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm new_syscall old_syscall

install:
	@sudo insmod -f modify_syscall.ko
	@echo "..."
	@sudo dmesg | tail -n 10 | grep modify_syscall

uninstall:
	@sudo rmmod -f modify_syscall
	@echo "..."
	@sudo dmesg | tail -n 10 | grep modify_syscall

.PHONY: show create install clean uninstall

get_syscall_table:
	sudo cat /proc/kallsyms | grep sys_call_table

test:
	gcc modify_new_syscall.c -o new_syscall
	gcc modify_old_syscall.c -o old_syscall
