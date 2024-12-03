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

jmp $


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
    db "Loading RougeOS ......... XD", 10, 13, 0
    
error_msg:
    db "Loading error! :(", 10, 13, 0

memory_detect_msg:
    db "Memory Detecting Success! :)", 10, 13, 0

;ards buffer's size is veriable, put it at the end of the file to avoid overflows
ards_count:
    dw 0
ards_buffer:
