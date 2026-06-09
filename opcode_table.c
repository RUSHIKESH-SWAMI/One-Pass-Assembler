#include <string.h>
#include "opcode_table.h"

struct Opcode {
    char name[10];
    int code;
};

struct Opcode optab[] = {
    {"LDA", 0x00}, {"LDX", 0x04}, {"STA", 0x0C}, 
    {"ADD", 0x18}, {"SUB", 0x1C}, {"JMP", 0x3C},
    {"STL", 0x14}, {"JSUB", 0x48}, {"COMP", 0x28},
    {"JEQ", 0x30}, {"RSUB", 0x4C}, {"JGT", 0x34},
    {"JLT", 0x38}, {"LDCH", 0x50}, {"STCH", 0x54}
};

int getOpcode(char *name) {
    int size = sizeof(optab) / sizeof(optab[0]);
    for (int i = 0; i < size; i++) {
        if (strcmp(optab[i].name, name) == 0) {
            return optab[i].code;
        }
    }
    return -1;
}
