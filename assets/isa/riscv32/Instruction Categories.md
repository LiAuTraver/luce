# Instruction Categories for RISC-V RV32I

> _note_: the `powershell` in the file extension is just for syntax highlighting purposes.

## Overview

### Instruction Format

```powershell
instruction  -> R_type
              | I_type
              | S_type
              | B_type
              | U_type
              | J_type
            ;
```

### Abbrs

- `rd` - destination register
- `rs1` - source register 1, `rs2` same for 2, ...
- `imm` - immediate value
- `opcode` - operation code, which specifies the type of instruction
- `funct3` or `funct7` - determines the operation of the instruction, e.g. `add`, `sub`, .... the `3` or `7` indicates the number of bits used for the field.

## Instruction Categories

### R-type

Register-register operations.

```powershell
R_type -> opcode rd funct3 rs1 rs2 funct7
```

Examples: `add`, `sub`, `and`, `or`, `xor`, `sll`, `srl`, `sra`, `slt`, `sltu`.

### I-type

Immediate operations.

```powershell
I_type -> opcode funct3 rd rs1 imm[11:0]
```

`imm[11:0]` is a 12-bit immediate value, sign-extended.

Examples: `addi`, `slti`, `sltiu`, `xori`, `ori`, `andi`, `slli`, `srli`, `srai`, `lb`, `lh`, `lw`, `lbu`, `lhu`.

### S-type

Store operations.

```powershell
S_type -> opcode imm[11:5] funct3 rs1 rs2 imm[4:0]
```

> note: the `imm` was split into two parts.

Examples: `sb`, `sh`, `sw`.

### B-type

Branch operations.

```powershell
B_type -> opcode imm[12] imm[10:5] funct3 rs1 rs2 imm[4:1] imm[11]
```

Conditional branches.

- `imm`: sign-extended, shifted left by 1 bit(not using `0`.). the immediate offset for the branch target.
- `funct3`: specifies the branch condition.

Examples: `beq`, `bne`, `blt`, `bge`, `bltu`, `bgeu`.

### U-type

Upper immediate operations, loads or computes an upper immediate value.

```powershell
U_type -> opcode rd imm[31:12]
```

Examples: `lui`, `auipc`.

### J-type

Jump operations, performs unconditional jumps.

```powershell
J_type -> opcode rd imm[20] imm[10:1] imm[11] imm[19:12];
```

## Notes

1. The `imm` field is sign-extended, meaning that the most significant bit is copied to the left to fill the remaining bits.
2. the actual bits stored on the machine are(must, according to the manual) in little-endian order, so the least significant byte is stored first.(e.g., as for `R_type`, the `funct7` is stored in the most significant byte, and `opcode` in the least significant byte.) ![Examples][instructionCategoryImage]

[instructionCategoryImage]: examples.png "from RISCV Manual"
