#include <rouge.h>

int magic = MAGICNUMBER;
char message[] = "Hello, World!";
char buffer[1024];

void kernel_init() {
    char *vedio_memory = (char *)0xb8000;
    for(int i = 0; i < sizeof(message); i++) {
        vedio_memory[i * 2] = message[i];
    }
}