#include <string.h>

struct Opcode {
    char name[10];
    int code;
};

struct Opcode optab[] = {
    {"LDA", 0x00}, {"LDX", 0x04}, {"STA", 0x0C}, 
    {"ADD", 0x18}, {"SUB", 0x1C}, {"JMP", 0x3C}
};

int getOpcode(char *name) {
    for (int i = 0; i < 6; i++) {
        if (strcmp(optab[i].name, name) == 0) {
            return optab[i].code;
        }
    }
    return -1;
}
