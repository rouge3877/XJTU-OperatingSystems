obj-m += modify_syscall.o

create: modify_syscall.c modify_new_syscall.c modify_old_syscall.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc modify_new_syscall.c -o modify_new_syscall
	gcc modify_old_syscall.c -o modify_old_syscall

install: modify_syscall.ko
	@sudo insmod modify_syscall.ko
	@echo "..."
	@sudo dmesg | tail -n 10 | grep modify_syscall

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm modify_new_syscall
	rm modify_old_syscall

uninstall:
	@sudo rmmod -f modify_syscall
	@echo "..."
	@sudo dmesg | tail -n 10 | grep modify_syscall

.PHONY: show create install clean uninstall
