#include "GData.h"
#include <stdlib.h>
#include <string.h>

static Instruction InstructionTable[6] = {
  {"addi", "001000", NULL},
  {"mul", "011100", "000010"},
  {"mult", "000000", "011000"},
  {"lui", "001111", NULL},
  {"lw", "100011", NULL},
  {"sub", "000000", "100010"}
};
Instruction* getInstruction(char* name) {
  int i;
  for (i = 0; i < 6; i++) {
    if (strcmp(name, InstructionTable[i].mnemonic) == 0) {
      return &InstructionTable[i];
    }
  }
  return NULL;
}

static Register RegisterTable[8] = {
  {"$t0", 8, "01000"},
  {"$t1", 9, "01001"},
  {"$t2", 10, "01010"},
  {"$t3", 11, "01011"},
  {"$s0", 16, "10000"},
  {"$s1", 17, "10001"},
  {"$s2", 18, "10010"},
  {"$s3", 19, "10011"}
};
Register* getRegister(char* name) {
  int i;
  for (i = 0; i < 8; i++) {
    if (strcmp(name, RegisterTable[i].name) == 0) {
      return &RegisterTable[i];
    }
  }
  return NULL;
}