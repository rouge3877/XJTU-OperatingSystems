#ifndef _IO_H_
#define _IO_H_

#include "type.h"

extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t data);

extern uint16_t inw(uint16_t port);
extern void outw(uint16_t port, uint16_t data);

#endif