obj-m += ascii_char_device.o

create: chat.c ascii_char_device.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc chat.c -o chat -lpthread

install: ascii_char_device.ko
	@sudo insmod ascii_char_device.ko
	@sudo mknod -m 666 /dev/ascii_char_device c 238 0
	@sudo chmod 666 /dev/ascii_char_device
	@echo "..."
	@sudo dmesg | tail -n 10 | grep ascii_char_device

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm chat

uninstall:
	@sudo rmmod ascii_char_device
	@sudo rm /dev/ascii_char_device
	@echo "..."
	@sudo dmesg | tail -n 10 | grep ascii_char_device

.PHONY: show create install clean uninstall

show: create install
	chmod +x show.sh
	./show.sh
