#include <rouge.h>
#include <io.h>
#include <type.h>


#define CRT_ADDR_REG 0x3D4
#define CRT_DATA_REG 0x3D5

#define CRT_CURSOR_HIGH 0x0E
#define CRT_CURSOR_LOW 0x0F

void kernel_init() {


    outb(CRT_ADDR_REG, CRT_CURSOR_HIGH);
    uint16_t pos = inb(CRT_DATA_REG) << 8;
    outb(CRT_ADDR_REG, CRT_CURSOR_LOW);
    pos |= inb(CRT_DATA_REG);


    outb(CRT_ADDR_REG, CRT_CURSOR_HIGH);
    outb(CRT_DATA_REG, 0);
    outb(CRT_ADDR_REG, CRT_CURSOR_LOW);
    outb(CRT_DATA_REG, 0); 

    return;
}