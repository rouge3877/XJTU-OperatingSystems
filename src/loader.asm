[org 0x1000]

; Check loader signature: "ROUGE" -> ASCII: 52 4F 55 47 45
db "ROUGE"

; Print booting message
mov si, booting
call print

; Detect memory regions using SMAP (System Management BIOS)
detect_memory:
    xor ebx, ebx                   ; Clear EBX register
    mov ax, 0                      ; Reset AX
    mov es, ax                     ; Set ES to 0
    mov edi, ards_buffer           ; ES:EDI points to ards_buffer
    mov edx, 0x534D4150             ; "SMAP" signature in EDX

.next:
    mov eax, 0xE820                ; BIOS call for memory map
    mov ecx, 20                    ; Size of each entry in bytes
    int 0x15                       ; BIOS interrupt 0x15

    jc error                        ; Jump to error if CF is set (carry flag)
    add di, cx                      ; Move to next memory region in buffer
    inc word [ards_count]           ; Increment memory region count

    cmp ebx, 0                      ; Check if we've processed all regions
    jnz .next                       ; Continue if not

; Print memory detection success message
mov si, memory_detect_msg
call print

; Prepare for protected mode (disable interrupts and set up A20 gate)
prepare_protected_mode:
    cli                             ; Disable interrupts
    in al, 0x92                     ; Enable A20 gate
    or al, 2
    out 0x92, al

    lgdt [gdt_pointer]              ; Load GDT (Global Descriptor Table)

    mov eax, cr0
    or eax, 1                       ; Set PG (Protection Enable) bit in CR0
    mov cr0, eax                    ; Write back to CR0

    ; Use far jump to flush the instruction cache
    jmp dword code_selector:protected_mode

; Print function (uses BIOS interrupt 0x10)
print:
    mov ah, 0x0e                    ; Teletype output function (BIOS)
.loop:
    mov al, [si]                    ; Load character from string
    cmp al, 0                       ; Check if end of string
    je .done
    int 0x10                        ; Print character
    inc si                           ; Move to next character
    jmp .loop
.done:
    ret

; Error handling (print error message and halt)
error:
    mov si, error_msg
    call print
    hlt                             ; Halt the system
    jmp $

; Switch to protected mode (32-bit)
[bits 32]
protected_mode:
    ; Initialize segment registers to the data selector
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up a good place to load the kernel at 0x7E00
    mov esp, 0x7E00                ; Set stack pointer

    ; Start loading the kernel into memory
    mov edi, 0x7E00                ; Set destination address for disk read
    mov ecx, 5                      ; Number of sectors to read
    mov bl, 200                     ; Number of sectors to read at once

    call read_disk                  ; Call disk reading function
    jmp dword code_selector:0x7E00  ; Jump to kernel's entry point

jmp $

; Read disk function (using BIOS INT 13h)
read_disk:
    ; Set the number of sectors to read (AH = 0x20 = read sectors)
    mov dx, 0x1f2                   ; I/O port 0x1f2 - sector count
    mov al, bl                      ; Set number of sectors to read
    out dx, al

    inc dx                          ; I/O port 0x1f3 - start sector (low byte)
    mov al, cl                      ; Start sector (low byte)
    out dx, al

    inc dx                          ; I/O port 0x1f4 - start sector (mid byte)
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx                          ; I/O port 0x1f5 - start sector (high byte)
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx                          ; I/O port 0x1f6 - LBA mode setup
    shr ecx, 8
    and cl, 0b1111                  ; Clear the high 4 bits
    mov al, 0b1110_0000
    or al, cl
    out dx, al

    inc dx                          ; I/O port 0x1f7 - command register
    mov al, 0x20                    ; Disk read command
    out dx, al

    xor ecx, ecx                    ; Clear ECX
    mov cl, bl                      ; Set sector count

.read:
    push cx                          ; Save CX register
    call .waits                      ; Wait for disk ready
    call .reads                      ; Read one sector
    pop cx                           ; Restore CX register
    loop .read                       ; Loop for all sectors
    ret

.waits:
    mov dx, 0x1f7                   ; I/O port 0x1f7 - status register
.check:
    in al, dx                       ; Read status register
    and al, 0b1000_1000             ; Check bits 7 and 3
    cmp al, 0b0000_1000             ; Check if data ready (bit 3)
    jnz .check                      ; Wait until ready
    ret

.reads:
    mov dx, 0x1f0                   ; I/O port 0x1f0 - data register
    mov cx, 256                     ; One sector = 256 words (512 bytes)
.readw:
    in ax, dx                       ; Read a word from the disk
    mov [edi], ax                   ; Store in memory
    add edi, 2                      ; Move to next memory location
    loop .readw                     ; Loop until entire sector is read
    ret

; Global Descriptor Table (GDT) for protected mode
code_selector equ (1<<3)
data_selector equ (2<<3)

; Memory base and limit
memory_base equ 0                 ; Memory base address
memory_limit equ ((1 << 32) / 4096 - 1) ; Memory limit address

; GDT pointer and descriptor entries
gdt_pointer:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_start:
    dd 0, 0                        ; Null descriptor

gdt_code:
    dw memory_limit & 0xffff        ; Limit (0:15)
    dw memory_base & 0xffff         ; Base (0:15)
    db (memory_base >> 16) & 0xff   ; Base (16:23)
    db 0b_1_00_1_1_0_1_0           ; Access byte
    db 0b1_1_0_0_0000 | ((memory_limit >> 16) & 0xf) ; Granularity & limit (16:19)
    db (memory_base >> 24) & 0xff   ; Base (24:31)

gdt_data:
    dw memory_limit & 0xffff        ; Limit (0:15)
    dw memory_base & 0xffff         ; Base (0:15)
    db (memory_base >> 16) & 0xff   ; Base (16:23)
    db 0b_1_00_1_0_0_1_0           ; Access byte
    db 0b1_1_0_0_0000 | ((memory_limit >> 16) & 0xf) ; Granularity & limit (16:19)
    db (memory_base >> 24) & 0xff   ; Base (24:31)

gdt_end:

; Boot message (will be printed during boot process)
booting:
    db "Loading RougeOS ......... XD", 10, 13, 0

; Error message (printed if loading fails)
error_msg:
    db "Loading error! :(", 10, 13, 0

; Memory detection success message
memory_detect_msg:
    db "Memory Detecting Success! :)", 10, 13, 0

; ARDS buffer and count (used for memory map detection)
ards_count:
    dw 0
ards_buffer:
