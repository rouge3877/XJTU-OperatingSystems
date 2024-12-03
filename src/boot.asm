[org 0x7c00]
; In the boot sector, the BIOS loads the boot sector into memory at 0x7c00
; So why do we need to set the origin to 0x7c00?
;    - For Compiler to know where to put the code (calculate the offset in right way)


; set screen mode: clear screen and set 80x25 text mode
; refer to IBM PS 2 and PC BIOS Interface Technical Reference
mov ax, 3
int 0x10

; initialize segment registers
mov ax, 0x0000
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov sp, 0x7c00

; 0xb800 is the start of the video memory
; refer to https://en.wikipedia.org/wiki/Video_Graphics_Array#Memory_layout
; mov ax, 0xb800
; mov ds, ax
; mov byte [0], 'A'
mov si, booting
call print

mov edi, 0x1000
mov ecx, 2
mov bl, 4
call read_disk

; check loader signature: cmp word[0x1000], 0x55aa
cmp word [0x1000], 0x55aa
jne error
jmp 0x1002


; mov edi, 0x1000
; mov ecx, 2
; mov bl, 1
; call write_disk

; block forever
jmp $

; ------------------------------
write_disk:

    ; 设置读写扇区的数量
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    inc dx; 0x1f3
    mov al, cl; 起始扇区的前八位
    out dx, al

    inc dx; 0x1f4
    shr ecx, 8
    mov al, cl; 起始扇区的中八位
    out dx, al

    inc dx; 0x1f5
    shr ecx, 8
    mov al, cl; 起始扇区的高八位
    out dx, al

    inc dx; 0x1f6
    shr ecx, 8
    and cl, 0b1111; 将高四位置为 0

    mov al, 0b1110_0000;
    or al, cl
    out dx, al; 主盘 - LBA 模式

    inc dx; 0x1f7
    mov al, 0x30; 写硬盘
    out dx, al

    xor ecx, ecx; 将 ecx 清空
    mov cl, bl; 得到读写扇区的数量

    .write:
        push cx; 保存 cx
        call .writes; 写入一个扇区
        call .waits; 等待数据准备完毕
        pop cx; 恢复 cx
        loop .write

    ret

    .waits:
        mov dx, 0x1f7
        .check:
            in al, dx
            jmp $+2; nop 直接跳转到下一行
            jmp $+2; 一点点延迟
            jmp $+2
            and al, 0b1000_0000
            cmp al, 0b0000_0000
            jnz .check
        ret

    .writes:
        mov dx, 0x1f0
        mov cx, 256; 一个扇区 256 字
        .writew:
            mov ax, [edi]
            out dx, ax
            jmp $+2; 一点点延迟
            jmp $+2
            jmp $+2

            add edi, 2
            loop .writew
        ret



read_disk:

    ; 设置读写扇区的数量
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    inc dx; 0x1f3
    mov al, cl; 起始扇区的前八位
    out dx, al

    inc dx; 0x1f4
    shr ecx, 8
    mov al, cl; 起始扇区的中八位
    out dx, al

    inc dx; 0x1f5
    shr ecx, 8
    mov al, cl; 起始扇区的高八位
    out dx, al

    inc dx; 0x1f6
    shr ecx, 8
    and cl, 0b1111; 将高四位置为 0

    mov al, 0b1110_0000;
    or al, cl
    out dx, al; 主盘 - LBA 模式

    inc dx; 0x1f7
    mov al, 0x20; 读硬盘
    out dx, al

    xor ecx, ecx; 将 ecx 清空
    mov cl, bl; 得到读写扇区的数量

    .read:
        push cx; 保存 cx
        call .waits; 等待数据准备完毕
        call .reads; 读取一个扇区
        pop cx; 恢复 cx
        loop .read

    ret

    .waits:
        mov dx, 0x1f7
        .check:
            in al, dx
            jmp $+2; nop 直接跳转到下一行
            jmp $+2; 一点点延迟
            jmp $+2
            and al, 0b1000_1000
            cmp al, 0b0000_1000
            jnz .check
        ret

    .reads:
        mov dx, 0x1f0
        mov cx, 256; 一个扇区 256 字
        .readw:
            in ax, dx
            jmp $+2; 一点点延迟
            jmp $+2
            jmp $+2
            mov [edi], ax
            add edi, 2
            loop .readw
        ret






print:
    mov ah, 0x0e
    .loop:
        mov al, [si]
        cmp al, 0
        je .done
        int 0x10
        inc si
        jmp .loop
    .done:
    ret

error:
    mov si, error_msg
    call print
    hlt
    jmp $

booting:
    ; 10 is the line feed, 13 is the carriage return
    db "Booting RougeOS ......... XD", 10, 13, 0
    
error_msg:
    db "Error: Something went wrong!", 10, 13, 0

; MBR: the last two bytes must be 0x55, 0xaa
; refer to https://en.wikipedia.org/wiki/Master_boot_record
times 510-($-$$) db 0
db 0x55, 0xaa
