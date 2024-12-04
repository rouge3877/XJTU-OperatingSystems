obj-m += globalvar.o

create: globalvar.c driverTest.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc driverTest.c -o driverTest

install: globalvar.ko
	@sudo insmod globalvar.ko
	@echo "..."
	@sudo dmesg | tail -n 10 | grep globalvar
	@sudo mknod -m 666 /dev/globalvar c 290 0
	@sudo chmod 666 /dev/globalvar

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm driverTest

uninstall:
	@sudo rmmod globalvar
	@sudo rm /dev/globalvar
	@echo "..."
	@sudo dmesg | tail -n 10 | grep globalvar
