[org 0x1000]

dw 0x55aa

mov si, booting
call print

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
    db "Booting RougeOS ......... XD", 10, 13, 0
    
error_msg:
    db "Error: Something went wrong!", 10, 13, 0