[org 0x1000]

; check loader signature: rouge -> ascii -> 52 4f 55 47 45
db "ROUGE"

mov si, booting
call print

detect_memory:
    xor ebx, ebx
    mov ax, 0
    mov es, ax
    mov edi, ards_buffer; es:edi = 0x0000:buffer
    mov edx, 0x534d4150; "SMAP"

    .next:
        mov eax, 0xe820
        mov ecx, 20
        int 0x15

        jc error; CF = 1, error
        add di, cx
        inc word [ards_count]

        cmp ebx, 0
        jnz .next

    mov si, memory_detect_msg
    call print

;     mov cx, [ards_count]
;     mov si, 0
; .show
;     mov eax, [ards_buffer + si]
;     mov ebx, [ards_buffer + si + 8]
;     mov edx, [ards_buffer + si + 16]
;     add si, 20

;     loop .show

; in protected mode, bios interrupts are disabled
prepare_protected_mode:
    cli

    in al, 0x92 ; open A20
    or al, 2
    out 0x92, al

    lgdt [gdt_pointer]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; use jmp to flush the instruction cache
    jmp dword code_selector:protected_mode


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


[bits 32]
protected_mode:
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000 ;

    mov byte [0xb8000], 'A'
    mov byte [0x200000], 'B'


jmp $


code_selector equ (1<<3)
data_selector equ (2<<3)

memory_base equ 0 ; memory base address
memory_limit equ ((1 << 32) / 4096 - 1) ; memory limit address

gdt_pointer:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_start:
    dd 0, 0; null descriptor
gdt_code:
    dw memory_limit & 0xffff; limit 0:15
    dw memory_base & 0xffff; base 0:15
    db (memory_base >> 16) & 0xff; base 16:23

    db 0b_1_00_1_1_0_1_0
    db 0b1_1_0_0_0000 | ((memory_limit >> 16) & 0xf)
    db (memory_base >> 24) & 0xff; base 24:31
gdt_data:
    dw memory_limit & 0xffff; limit 0:15
    dw memory_base & 0xffff; base 0:15
    db (memory_base >> 16) & 0xff; base 16:23

    db 0b_1_00_1_0_0_1_0
    db 0b1_1_0_0_0000 | ((memory_limit >> 16) & 0xf)
    db (memory_base >> 24) & 0xff; base 24:31
gdt_end:

booting:
    ; 10 is the line feed, 13 is the carriage return
    db "Loading RougeOS ......... XD", 10, 13, 0
    
error_msg:
    db "Loading error! :(", 10, 13, 0

memory_detect_msg:
    db "Memory Detecting Success! :)", 10, 13, 0

;ards buffer's size is veriable, put it at the end of the file to avoid overflows
ards_count:
    dw 0
ards_buffer:
