[org 0x7c00]  ; The BIOS loads the boot sector into memory at 0x7c00
; In the boot sector, the BIOS loads the boot sector into memory at 0x7c00
; So why do we need to set the origin to 0x7c00?
;    - For Compiler to know where to put the code (calculate the offset in right way)


; Set video mode: 80x25 text mode (clear screen)
mov ax, 3
int 0x10

; Initialize segment registers (set them to 0)
mov ax, 0x0000
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov sp, 0x7c00  ; Set stack pointer to the top of boot sector

; Display the boot message
mov si, booting
call print

; Read first sector from disk (assume it's a bootloader)
mov edi, 0x1000     ; Destination buffer
mov ecx, 1          ; Read 1 sector
mov bl, 4           ; LBA read mode (4 sectors)
call read_disk

; Check loader signature ("ROUGE") at 0x1000
cmp byte [0x1000], 0x52  ; 'R'
jne error
cmp byte [0x1001], 0x4F  ; 'O'
jne error
cmp byte [0x1002], 0x55  ; 'U'
jne error
cmp byte [0x1003], 0x47  ; 'G'
jne error
cmp byte [0x1004], 0x45  ; 'E'
jne error

; Jump to loader at 0x1005
jmp 0x1005

; Infinite loop in case of error
jmp $

; Write disk function (writes one sector)
write_disk:
    ; Set the number of sectors to write
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    ; Set the starting sector (low byte)
    inc dx
    mov al, cl
    out dx, al

    ; Set the starting sector (middle byte)
    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al

    ; Set the starting sector (high byte)
    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al

    ; Set the disk mode to LBA
    inc dx
    shr ecx, 8
    and cl, 0b1111          ; Clear high 4 bits
    mov al, 0b1110_0000
    or al, cl
    out dx, al              ; Main disk - LBA mode

    ; Disk write command
    inc dx
    mov al, 0x30            ; Write sectors
    out dx, al

    ; Set ECX to the number of sectors to write
    xor ecx, ecx
    mov cl, bl

    ; Write loop
.write:
    push cx
    call .writes            ; Write one sector
    call .waits             ; Wait for data to be ready
    pop cx
    loop .write

    ret

; Wait for the disk to be ready
.waits:
    mov dx, 0x1f7           ; Status register
.check:
    in al, dx
    and al, 0b1000_0000     ; Check bit 7 (drive ready)
    cmp al, 0b0000_0000     ; If not ready, continue checking
    jnz .check
    ret

; Write one sector
.writes:
    mov dx, 0x1f0           ; Data register
    mov cx, 256             ; One sector = 256 words (512 bytes)
.writew:
    mov ax, [edi]
    out dx, ax
    add edi, 2              ; Move to next word
    loop .writew
    ret

; Read disk function (reads one sector)
read_disk:
    ; Set the number of sectors to read
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    ; Set the starting sector (low byte)
    inc dx
    mov al, cl
    out dx, al

    ; Set the starting sector (middle byte)
    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al

    ; Set the starting sector (high byte)
    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al

    ; Set the disk mode to LBA
    inc dx
    shr ecx, 8
    and cl, 0b1111          ; Clear high 4 bits
    mov al, 0b1110_0000
    or al, cl
    out dx, al              ; Main disk - LBA mode

    ; Disk read command
    inc dx
    mov al, 0x20            ; Read sectors
    out dx, al

    ; Set ECX to the number of sectors to read
    xor ecx, ecx
    mov cl, bl

    ; Read loop
.read:
    push cx
    call .waits             ; Wait for data to be ready
    call .reads             ; Read one sector
    pop cx
    loop .read

    ret

; Wait for the disk to be ready
.waits:
    mov dx, 0x1f7           ; Status register
.check:
    in al, dx
    and al, 0b1000_1000     ; Check bit 7 (drive ready)
    cmp al, 0b0000_1000     ; If data is ready (bit 3)
    jnz .check
    ret

; Read one sector
.reads:
    mov dx, 0x1f0           ; Data register
    mov cx, 256             ; One sector = 256 words (512 bytes)
.readw:
    in ax, dx
    mov [edi], ax           ; Store word in memory
    add edi, 2              ; Move to next word
    loop .readw
    ret

; Print string using BIOS interrupt
print:
    mov ah, 0x0e            ; Teletype output function (BIOS)
.loop:
    mov al, [si]
    cmp al, 0               ; Check if it's the end of the string
    je .done
    int 0x10                ; Print character
    inc si                   ; Move to next character
    jmp .loop
.done:
    ret

; Error handling: print error message and halt
error:
    mov si, error_msg
    call print
    hlt
    jmp $

; Boot message (will be displayed during boot process)
booting:
    db "Booting RougeOS ......... XD", 10, 13, 0

; Error message (printed if loader signature check fails)
error_msg:
    db "Error: Something went wrong!", 10, 13, 0

; Master Boot Record (MBR): last two bytes must be 0x55, 0xaa
; Refer to https://en.wikipedia.org/wiki/Master_boot_record
times 510-($-$$) db 0
db 0x55, 0xaa  ; MBR signature
