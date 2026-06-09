#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "opcode_table.h"

void onePassAssembler(FILE *inputFile, FILE *outputFile) {
    char line[100];
    int locctr = 0, startAddress = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        char label[20] = "", opcode[20] = "", operand[20] = "";
        
        // Remove trailing carriage returns and newlines
        line[strcspn(line, "\r\n")] = '\0';
        
        // Skip comment lines or empty lines
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0' || *ptr == '.') continue;

        // Determine if statement starts with a label (no leading whitespace)
        int hasLabel = (line[0] != ' ' && line[0] != '\t');
        int fields = 0;

        if (hasLabel) {
            fields = sscanf(line, "%s %s %s", label, opcode, operand);
            if (fields >= 2 && label[0] != '.' && opcode[0] != '.') {
                if (operand[0] == '.') {
                    strcpy(operand, "");
                }
                insertSymbol(label, locctr);
                resolveForwardReferences(label, locctr, outputFile);
            }
        } else {
            fields = sscanf(line, "%s %s", opcode, operand);
            if (fields >= 1 && opcode[0] != '.') {
                if (operand[0] == '.') {
                    strcpy(operand, "");
                }
            }
        }

        // If no valid opcode was parsed, skip
        if (strlen(opcode) == 0) {
            continue;
        }

        if (strcmp(opcode, "START") == 0) {
            startAddress = (int)strtol(operand, NULL, 16);
            locctr = startAddress;
            fprintf(outputFile, "H^%-6s^%06X^000000\n", label, startAddress);
            continue;
        }

        if (strcmp(opcode, "END") == 0) {
            int programLength = locctr - startAddress;
            // Seek to offset 16 (H^%-6s^%06X^ starting point) and write length
            fseek(outputFile, 16, SEEK_SET);
            fprintf(outputFile, "%06X", programLength);
            
            // Return to end of file
            fseek(outputFile, 0, SEEK_END);
            
            // Determine execution start address
            int endAddress = startAddress;
            if (strlen(operand) > 0) {
                int targetAddr = getSymbolAddress(operand);
                if (targetAddr != -1) {
                    endAddress = targetAddr;
                }
            }
            fprintf(outputFile, "E^%06X\n", endAddress);
            break;
        }

        int opCodeVal = getOpcode(opcode);
        if (opCodeVal != -1) {
            if (strcmp(opcode, "RSUB") == 0) {
                fprintf(outputFile, "T^%06X^03^4C0000\n", locctr);
            } else {
                int operandAddr = getSymbolAddress(operand);
                if (operandAddr == -1) {
                    // Forward reference: write instruction with placeholder address 0000
                    addForwardReference(operand, locctr + 1);
                    fprintf(outputFile, "T^%06X^03^%02X0000\n", locctr, opCodeVal);
                } else {
                    fprintf(outputFile, "T^%06X^03^%02X%04X\n", locctr, opCodeVal, operandAddr);
                }
            }
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += 3 * atoi(operand);
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(opcode, "WORD") == 0) {
            fprintf(outputFile, "T^%06X^03^%06X\n", locctr, (int)strtol(operand, NULL, 10));
            locctr += 3;
        } else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'C' && operand[1] == '\'') {
                // Character constant like C'EOF'
                int len = 0;
                char hexStr[100] = "";
                for (int i = 2; operand[i] != '\'' && operand[i] != '\0'; i++) {
                    char temp[3];
                    sprintf(temp, "%02X", operand[i]);
                    strcat(hexStr, temp);
                    len++;
                }
                fprintf(outputFile, "T^%06X^%02X^%s\n", locctr, len, hexStr);
                locctr += len;
            } else if (operand[0] == 'X' && operand[1] == '\'') {
                // Hex constant like X'F1'
                int len = 0;
                char hexStr[100] = "";
                for (int i = 2; operand[i] != '\'' && operand[i] != '\0'; i++) {
                    hexStr[len] = operand[i];
                    len++;
                }
                hexStr[len] = '\0';
                int byteLen = len / 2;
                fprintf(outputFile, "T^%06X^%02X^%s\n", locctr, byteLen, hexStr);
                locctr += byteLen;
            } else {
                // Plain number
                fprintf(outputFile, "T^%06X^01^%02X\n", locctr, (int)strtol(operand, NULL, 10));
                locctr += 1;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char *inputFileName = "input.asm";
    char *outputFileName = "output.obj";

    if (argc >= 2) {
        inputFileName = argv[1];
    }
    if (argc >= 3) {
        outputFileName = argv[2];
    }

    FILE *inputFile = fopen(inputFileName, "r");
    FILE *outputFile = fopen(outputFileName, "w+");

    if (!inputFile || !outputFile) {
        printf("Error opening files: %s or %s.\n", inputFileName, outputFileName);
        if (inputFile) fclose(inputFile);
        if (outputFile) fclose(outputFile);
        return 1;
    }

    onePassAssembler(inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);
    printf("Assembly completed successfully.\n");
    return 0;
}
