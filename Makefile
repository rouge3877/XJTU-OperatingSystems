# Compile boot.asm
build/boot.bin: src/boot.asm
	mkdir -p build
	nasm -f bin src/boot.asm -o build/boot.bin

# Compile loader.asm
build/loader.bin: src/loader.asm
	mkdir -p build
	nasm -f bin src/loader.asm -o build/loader.bin

# Create hard disk image
os.img: build/boot.bin build/loader.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat -q build/os.img
	dd if=build/boot.bin of=build/os.img bs=512 count=1 seek=0 conv=notrunc
	dd if=build/loader.bin of=build/os.img bs=512 count=4 seek=2 conv=notrunc

# Run the OS on QEMU
run: os.img
	qemu-system-i386  -monitor stdio -drive file=build/os.img,format=raw,index=0,media=disk

debug: os.img
	qemu-system-i386 -s -S ./build/os.img &
	gdb -x script/init.gdb


.PHONY: clean run debug

clean:
	rm -rf build/