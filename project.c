// I have not used C language code obtained from other students, the 
// Internet, and any other unauthorized sources, either modified or 
// unmodified. If any code in my program was obtained from an authorized 
// source, such as textbook or course notes, that has been clearly noted 
// as a citation in the comments of the program. 
// <Samuel Sklar> 
// <sa059867@ucf.edu>
#include "spimcore.h"

/* ALU */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero) {
    switch (ALUControl) {
        case 0b000: // Z = A + B
            *ALUresult = A + B;
            break;
        case 0b001: // Z = A - B
            *ALUresult = A - B;
            break;
        case 0b010: // Z = 1 if A < B, else Z = 0
            *ALUresult = (A < B) ? 1 : 0;
            break;
        case 0b011: // Z = 1 if A < B (unsigned), else Z = 0
            *ALUresult = (unsigned)A < (unsigned)B ? 1 : 0;
            break;
        case 0b100: // Z = A AND B
            *ALUresult = A & B;
            break;
        case 0b101: // Z = A OR B
            *ALUresult = A | B;
            break;
        case 0b110: // Shift left B by 16 bits
            *ALUresult = B << 16;
            break;
        case 0b111: // Z = NOT A
            *ALUresult = ~A;
            break;
        default:
            // Set ALUresult and Zero to default values
            *ALUresult = 0;
            *Zero = 1;
            break;
    }
    // Determine if the result is zero
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* Instruction Fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Check if PC is aligned properly
    if (PC % 4 != 0)
        return 1;

    // Fetch instruction from memory
    *instruction = Mem[PC >> 2];

    return 0;
}

/* Instruction Partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    //Extracting different fields from the instruction (opCode, firstRegister, secondReg, thirdReg, function, offset, and jump)
    *op = (instruction >> 26) & 0x3F;
    *r1 = (instruction >> 21) & 0x1F;
    *r2 = (instruction >> 16) & 0x1F;
    *r3 = (instruction >> 11) & 0x1F;
    *funct = instruction & 0x0000003f;
    *offset = instruction & 0x0000ffff;
    *jsec = instruction & 0x03ffffff;
}

/* Instruction Decode */
/* 30 Points */
int instruction_decode(unsigned op, struct_controls *controls) {
    // Initialize control signals to default values
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0; 
    controls->ALUOp = 0; // Default value for ALU operations
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;

    // Decode instruction to control signals based on opcode
    switch (op) {
        // R-type instruction
        case 0x0:
            controls->RegDst = 1;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 7;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 1;
            return 0;
        // J-type instruction (Jump)
        case 0x2:
            controls->RegDst = 0;
            controls->Jump = 1;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 0;
            return 0;
        // I-type instruction (Branch Equal)
        case 0x4:
            controls->RegDst = 2; // Don't Care
            controls->Jump = 0;
            controls->Branch = 1;
            controls->MemRead = 0;
            controls->MemtoReg = 2; // Don't Care
            controls->ALUOp = 1;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 0;
            return 0;
        // Load word instruction
        case 0x23:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        // Store word instruction
        case 0x2B:
            controls->RegDst = 2; // Don't Care
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 2; // Don't Care
            controls->ALUOp = 0;
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            return 0;
        // I-type instruction (Add Immediate)
        case 0x8:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        // I-type instruction (Set Less Than Immediate)
        case 0xA:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 2;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        // I-type instruction (Set Less Than Immediate Unsigned)
        case 0xB:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 3;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        // I-type instruction (Load Upper Immediate)
        case 0xF:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 6; // Left shift immediate value by 16 bits
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        default:
            return 1;
    }
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    // Reads registers
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    // Sign extend the 16-bit offset to 32 bits
    if (offset & 0x8000) // If sign bit is set
        *extended_value = offset | 0xFFFF0000;
    else
        *extended_value = offset;
}

/* ALU Operations */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero) {
    // Handle second parameter to ALU
    unsigned B = ALUSrc == 1 ? extended_value : data2;

    // Handle ALU control unit
    if (ALUOp == 7) {
        // Add
        if (funct == 0b100000)
            ALUOp = 0;
        // Subtract
        else if (funct == 0b100010)
            ALUOp = 1;
        // And
        else if (funct == 0b100100)
            ALUOp = 4;
        // Or
        else if (funct == 0b100101)
            ALUOp = 5;
        // Slt
        else if (funct == 0b101010)
            ALUOp = 2;
        // Sltu
        else if (funct == 0b101011)
            ALUOp = 3;
        // Unknown funct code
        else
            return 1;
    }

    ALU(data1, B, ALUOp, ALUresult, Zero);
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem) {
    if (MemRead) { // Read from memory
        if (ALUresult % 4 != 0) // Check if ALUresult is word-aligned
            return 1;
        *memdata = Mem[ALUresult >> 2];
    }

    if (MemWrite) { // Write to memory
        if (ALUresult % 4 != 0) // Check if ALUresult is word-aligned
            return 1;
        Mem[ALUresult >> 2] = data2;
    }

    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg) {
    unsigned reg_index = RegDst ? r3 : r2;

    if (RegWrite) {
        if (reg_index != 0) { // Check if the register is not $zero
            if (MemtoReg)
                Reg[reg_index] = memdata;
            else
                Reg[reg_index] = ALUresult;
        }
    }
}

/* PC Update */
/* 15 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC) {
    // Update PC with PC+4
    *PC += 4;

    // Check conditions for updating PC
    if (Branch && Zero) {
        // Branch taken: Update PC according to previous PC and extended_value
        *PC += extended_value << 2; // Shift left by 2 bits for word addressing
    } else if (Jump) {
        // Jump: Update PC according to previous PC and jsec
        *PC = (*PC & 0xF0000000) | (jsec << 2); // Preserve high bits, update low bits with jsec
    }
}
