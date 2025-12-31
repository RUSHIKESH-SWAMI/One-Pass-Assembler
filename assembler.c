#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "opcode_table.h"

void onePassAssembler(FILE *inputFile, FILE *outputFile) {
    char line[100], label[20], opcode[20], operand[20];
    int locctr = 0, startAddress = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        int fields = sscanf(line, "%s %s %s", label, opcode, operand);

        if (fields == 3) { // Label present
            insertSymbol(label, locctr);
        } else if (fields == 2) { // No label
            strcpy(operand, opcode);
            strcpy(opcode, label);
            strcpy(label, "");
        }

        if (strcmp(opcode, "START") == 0) {
            startAddress = atoi(operand);
            locctr = startAddress;
            fprintf(outputFile, "H^%s^%06d\n", label, startAddress);
            continue;
        }

        if (strcmp(opcode, "END") == 0) {
            fprintf(outputFile, "E^%06d\n", startAddress);
            break;
        }

        int opCodeVal = getOpcode(opcode);
        if (opCodeVal != -1) {
            int operandAddr = getSymbolAddress(operand);
            if (operandAddr == -1) {
                // Forward reference handling: add to list to be patched later
                addForwardReference(operand, locctr + 1);
                fprintf(outputFile, "T^%06d^02^%02X0000\n", locctr, opCodeVal);
            } else {
                fprintf(outputFile, "T^%06d^02^%02X%04X\n", locctr, opCodeVal, operandAddr);
            }
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += 3 * atoi(operand);
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(opcode, "WORD") == 0) {
            fprintf(outputFile, "T^%06d^02^%06X\n", locctr, atoi(operand));
            locctr += 3;
        } else if (strcmp(opcode, "BYTE") == 0) {
            fprintf(outputFile, "T^%06d^02^%02X\n", locctr, atoi(operand));
            locctr += 1;
        }
    }
}

int main() {
    FILE *inputFile = fopen("input.asm", "r");
    FILE *outputFile = fopen("output.obj", "w");

    if (!inputFile || !outputFile) {
        printf("Error opening files.\n");
        return 1;
    }

    onePassAssembler(inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);
    printf("Assembly completed successfully.\n");
    return 0;
}
