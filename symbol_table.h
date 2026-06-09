#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>

void insertSymbol(char *name, int address);
int getSymbolAddress(char *name);
void addForwardReference(char *name, int loc);
void resolveForwardReferences(char *name, int symbolAddress, FILE *outputFile);

#endif
