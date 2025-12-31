#include <string.h>
#include <stdio.h>
#include "symbol_table.h"

#define MAX_SYMBOLS 100

struct Symbol {
    char name[20];
    int address;
} symbolTable[MAX_SYMBOLS];

int symbolCount = 0;

void insertSymbol(char *name, int address) {
    strcpy(symbolTable[symbolCount].name, name);
    symbolTable[symbolCount].address = address;
    symbolCount++;
}

int getSymbolAddress(char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].address;
        }
    }
    return -1;
}

// Simple linked list for forward references
struct ForwardRef {
    char name[20];
    int patchAddr;
    struct ForwardRef *next;
} *head = NULL;

void addForwardReference(char *name, int loc) {
    struct ForwardRef *newRef = malloc(sizeof(struct ForwardRef));
    strcpy(newRef->name, name);
    newRef->patchAddr = loc;
    newRef->next = head;
    head = newRef;
}
