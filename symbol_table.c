#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

void resolveForwardReferences(char *name, int symbolAddress, FILE *outputFile) {
    struct ForwardRef *current = head;
    struct ForwardRef *prev = NULL;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Write patch record
            fprintf(outputFile, "T^%06X^02^%04X\n", current->patchAddr, symbolAddress);
            
            // Remove node from linked list
            struct ForwardRef *temp = current;
            if (prev == NULL) {
                head = current->next;
            } else {
                prev->next = current->next;
            }
            current = current->next;
            free(temp);
        } else {
            prev = current;
            current = current->next;
        }
    }
}
