obj-m += ascii_char_device.o

create:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc chat.c -o chat -lpthread

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod ascii_char_device.ko
	sudo dmesg | tail
	sudo mknod -m 666 /dev/ascii_char_device c 238 0
	sudo chmod 666 /dev/ascii_char_device

uninstall:
	sudo rmmod ascii_char_device
	sudo rm /dev/ascii_char_device
	sudo dmesg | tail
