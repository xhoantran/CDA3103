#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
  switch (ALUControl) {
    case 0x0:
      *ALUresult = A + B;
      break;
    case 0x1:
      *ALUresult = A - B;
      break;
    case 0x2:
      *ALUresult = ((int)A < (int)B) ? 1 : 0;
      break;
    case 0x3:
      *ALUresult = (A < B) ? 1 : 0;
      break;
    case 0x4:
      *ALUresult = A & B;
      break;
    case 0x5:
      *ALUresult = A | B;
      break;
    case 0x6:
      *ALUresult = B << 16;
      break;
    case 0x7:
      *ALUresult = ~A;
      break;
  }

  *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
  if (PC % 4 != 0) {
    return 1;
  }

  *instruction = Mem[PC >> 2];
  return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned 
*r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
  *op = instruction >> 26;
  *r1 = (instruction >> 21) & 0x1F;
  *r2 = (instruction >> 16) & 0x1F;
  *r3 = (instruction >> 11) & 0x1F;
  *funct = instruction & 0x3F;
  *offset = instruction & 0xFFFF;
  *jsec = instruction & 0x3FFFFFF;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
  controls->RegDst = 0;
	controls->Jump = 0;
	controls->Branch = 0;
	controls->MemRead = 0;
	controls->MemtoReg = 0;
	controls->ALUOp = 0;
	controls->MemWrite = 0;
	controls->ALUSrc = 0;
	controls->RegWrite = 0;

  switch(op) {
    // R-Type Instructions (add, sub, and, or, slt, sltu) - 00 0000
    case 0x0:
      controls->RegDst = 1;
      controls->ALUOp = 7;
      controls->RegWrite = 1;
      break;

    // Add Immediate (addi) - 00 1000
    case 0x8:
      controls->ALUSrc = 1;
      controls->RegWrite = 1;
      break;

    // Load Word (lw) - 10 0011
    case 0x23:
      controls->MemRead = 1;
      controls->MemtoReg = 1;
      controls->ALUSrc = 1;
      controls->RegWrite = 1;
      break;

    // Store Word (sw) - 10 1011
    case 0x2B:
      controls->RegDst = 2;
      controls->MemtoReg = 2;
      controls->MemWrite = 1;
      controls->ALUSrc = 1;
      break;

    // Load Upper Immediate (lui) - 00 1111
    case 0xF:
      controls->ALUOp = 6;
      controls->ALUSrc = 1;
      controls->RegWrite = 1;
      break;

    // Branch on Equal (beq) - 00 0100
    case 0x4:
      controls->RegDst = 2;
      controls->Branch = 1;
      controls->MemtoReg = 2;
      controls->ALUOp = 1;
      break;

    // Set less than immediate (slti) - 00 1010
    case 0xA:
      controls->ALUOp = 2;
      controls->ALUSrc = 1;
      controls->RegWrite = 1;
      break;

    // Set less than immediate unsigned (sltiu) - 00 1011
    case 0xB:
      controls->ALUOp = 3;
      controls->ALUSrc = 1;
      controls->RegWrite = 1;
      break;

    // Jump (j) - 00 0010
    case 0x2:
      controls->RegDst = 2;
      controls->Jump = 1;
      controls->MemRead = 2;
      controls->MemtoReg = 2;
      controls->ALUSrc = 2;
      break;

    default:
      return 1;
  }

  return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned 
*data2)
{
  *data1 = Reg[r1];
  *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
  *extended_value = (offset & 0x8000) ? (offset | 0xFFFF0000) : (offset & 0x0000FFFF);
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned 
funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  if (ALUOp < 0 || ALUOp > 7) {
    return 1;
  }

  if (ALUOp == 7) {
    switch (funct) {
      case 0x20:
        ALUOp = 0; // add
        break;
      case 0x22:
        ALUOp = 1; // sub
        break;
      case 0x24:
        ALUOp = 4; // and
        break;
      case 0x25:
        ALUOp = 5; // or
        break;
      case 0x2A:
        ALUOp = 2; // slt
        break;
      case 0x2B:
        ALUOp = 3; // sltu
        break;
      default:
        return 1;
    }
  }

  unsigned B = (ALUSrc == 1) ? extended_value : data2;

  ALU(data1, B, ALUOp, ALUresult, Zero);

  return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned
*memdata,unsigned *Mem)
{
  if (MemRead == 1) {
    if (ALUresult % 4 != 0) {
      return 1;
    }

    *memdata = Mem[ALUresult >> 2];
  }

  if (MemWrite == 1) {
    if (ALUresult % 4 != 0) {
      return 1;
    }

    Mem[ALUresult >> 2] = data2;
  }

  return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned 
ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
  if (RegWrite == 1) {
    if (MemtoReg == 1) {
      Reg[r2] = memdata;
    } else if (MemtoReg == 0) {
      if (RegDst == 1) {
        Reg[r3] = ALUresult;
      } else if (RegDst == 0) {
        Reg[r2] = ALUresult;
      }
    }
  }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char 
Zero,unsigned *PC)
{
  *PC += 4;

  if (Branch == 1 && Zero == 1) {
    *PC += extended_value << 2;
  }

  if (Jump == 1) {
    *PC = (*PC & 0xF0000000) | (jsec << 2);
  }
}
