# Directories for source and build files
BUILD := build
SRC := src

# gcc
CC := gcc
CFLAGS := -m32
CFLAGE += -fno-buildin # disable gcc built-in functions
CFLAGS += -nostdinc # disable standard header
CFLAGS += -fno-pic # disable position independent code
CFLAGS += -fno-pie # disable position independent executable
CFLAGS += -nostdlib # disable standard library
CFLAGS += -fno-stack-protector # disable stack protector
CFLAGS := $(strip ${CFLAGS})

DEBUG := -g
INCLUDE := -I$(SRC)/include

# QEMU configuration variables
QEMU := qemu-system-i386
QEMU_MONITOR := -monitor stdio
QEMU_OPTIONS := -m 32M -boot order=c
QEMU_DEBUG := -s -S

# GDB configuration variables
GDB := gdb
GDB_SCRIPT := script/init.gdb

# bximage configuration variables
BXIMAGE := bximage
BXIMAGE_OPTIONS := -q -hd=16 -func=create -sectsize=512 -imgmode=flat


ENTRY_POINT := 0x7E00

# Ensure build directory exists
$(shell mkdir -p $(BUILD))

# Compile boot.asm and loader.asm into binary files
$(BUILD)/%.bin: $(SRC)/%.asm
	@echo "Assembling $< into $@"
	nasm -f bin $< -o $@

# Compile kernel
$(BUILD)/kernel/%.o: $(SRC)/kernel/%.asm
	$(shell mkdir -p $(BUILD)/kernel)
	nasm -f elf32 $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.c
	$(shell mkdir -p $(BUILD)/kernel)
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

# Link kernel
$(BUILD)/kernel.bin: $(BUILD)/kernel/start.o \
					 $(BUILD)/kernel/main.o \
					 $(BUILD)/kernel/io.o \
	$(shell mkdir -p $(BUILD)/kernel)
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRY_POINT)

$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $^ $@

$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $< | sort > $@


# Create a hard disk image
$(BUILD)/os.img: $(BUILD)/boot.bin \
				 $(BUILD)/loader.bin \
				 $(BUILD)/system.bin \
				 $(BUILD)/system.map \

	@echo "Creating hard disk image $(BUILD)/os.img"
	# Create the empty disk image with bximage
	yes | $(BXIMAGE) $(BXIMAGE_OPTIONS) $(BUILD)/os.img
	
	# Write boot.bin to the first 512-byte sector (boot sector)
	dd if=$(BUILD)/boot.bin of=$(BUILD)/os.img bs=512 count=1 seek=0 conv=notrunc
	# Write loader.bin to the second sector and the following 3 sectors (total of 4 sectors)
	dd if=$(BUILD)/loader.bin of=$(BUILD)/os.img bs=512 count=4 seek=1 conv=notrunc
	# Write system.bin to the 6th sector and the following 20 sectors (total of 200 sectors)
	dd if=$(BUILD)/system.bin of=$(BUILD)/os.img bs=512 count=200 seek=5 conv=notrunc

# Run the OS on QEMU
qemu: $(BUILD)/os.img
	@echo "Running the OS with QEMU"
	$(QEMU) -drive file=$<,format=raw,index=0,media=disk \
		$(QEMU_OPTIONS) $(QEMU_MONITOR)

# Debug the OS using GDB with QEMU
qemu-debug: $(BUILD)/os.img
	@echo "Starting QEMU with GDB for debugging"
	# Start QEMU with remote debugging enabled
	$(QEMU) -drive file=$<,format=raw,index=0,media=disk \
		$(QEMU_OPTIONS) $(QEMU_DEBUG) &
	# Run GDB with the specified script
	$(GDB) -x $(GDB_SCRIPT)

# Clean the build directory
clean:
	@echo "Cleaning the build directory..."
	rm -rf $(BUILD)

# .PHONY declaration for non-file targets
.PHONY: clean qemu qemu-debug
