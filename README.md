# One-Pass Assembler for SIC

A professional C implementation of a **One-Pass Assembler** for the Simplified Instructional Computer (SIC) architecture. This assembler parses SIC assembly source code and outputs machine object code in a single pass, utilizing a dynamic back-patching mechanism to resolve forward references.

## Features

- **Single Pass Translation**: Generates object code on the fly without requiring a second pass over the source file.
- **Forward Reference Resolution**: Emits patch Text records (`T`) to dynamically update memory addresses once forward-referenced labels are defined (back-patching).
- **Directives Support**: Implements standard SIC directives:
  - `START` / `END`
  - `WORD` / `BYTE` (supporting character `C'...'` and hexadecimal `X'...'` constant formats)
  - `RESW` / `RESB`
- **Expanded Instruction Set**: Built-in support for common SIC instructions: `LDA`, `LDX`, `STA`, `ADD`, `SUB`, `JMP`, `STL`, `JSUB`, `COMP`, `JEQ`, `RSUB`, `JGT`, `JLT`, `LDCH`, `STCH`.
- **Seek-based Header Patching**: Automatically calculates the total program length and uses seek-based file updating to patch the Header (`H`) record.
- **Robust Syntax Parsing**: Skips comment lines (starting with `.`) and correctly processes statements with 1, 2, or 3 tokens.
- **Command-line Interface**: Custom input and output filenames can be passed as command line arguments.

---

## File Structure

- `assembler.c`: Main assembler logic, line parsing, location counter tracking, and file formatting.
- `symbol_table.c` / `symbol_table.h`: Symbol table management and forward reference linked list logic.
- `opcode_table.c` / `opcode_table.h`: Opcode table lookup for SIC instructions.

---

## Getting Started

### Prerequisites
A C compiler (e.g., `gcc`) installed on your system.

### Compilation
Compile all source files using a standard C compiler:
```bash
gcc -Wall assembler.c symbol_table.c opcode_table.c -o assembler
```

### Usage
Run the assembler by specifying the input assembly file and the target output object file:
```bash
./assembler [input_file.asm] [output_file.obj]
```
If no arguments are provided, the assembler defaults to `input.asm` as the input and `output.obj` as the output.

---

## Example

### Input Program (`input.asm`)
```assembly
COPY    START   1000
FIRST   LDA     THREE
        ADD     FIVE
        STA     ALPHA
        JMP     NEXT
ALPHA   RESW    1
FIVE    WORD    5
THREE   WORD    3
NEXT    LDA     ALPHA
        RSUB
        END     FIRST
```

### Generated Object Program (`output.obj`)
```text
H^COPY  ^001000^00001B
T^001000^03^000000
T^001003^03^180000
T^001006^03^0C0000
T^001009^03^3C0000
T^001007^02^100C
T^001004^02^100F
T^00100F^03^000005
T^001001^02^1012
T^001012^03^000003
T^00100A^02^1015
T^001015^03^00100C
T^001018^03^4C0000
E^001000
```

### Object Record Format Description
1. **Header Record (`H`)**: `H^[Name]^[Start Address]^[Program Length]`
2. **Text Record (`T`)**: `T^[Start Address]^[Record Length]^[Object Code]`
3. **Patch Record (Forward Reference Resolution)**: E.g., `T^001007^02^100C` indicates that a 2-byte address patch with the value `100C` needs to be applied to the instruction's operand field located at address `001007`.
4. **End Record (`E`)**: `E^[First Executable Address]`
