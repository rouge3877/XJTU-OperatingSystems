#ifndef _CONCOLE_H_
#define _CONCOLE_H_

#include "type.h"

void console_clear();
void console_init();
void console_write(char *buffer, uint32_t size);

#endif