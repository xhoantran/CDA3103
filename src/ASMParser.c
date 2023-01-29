#include "ASMParser.h"
#include "GData.h"
/***  Add include directives for here as needed.  ***/

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int skipWhiteSpace(char *line, int index) {
    while (line[index] == ' ' || line[index] == '\t') {
        index++;
    }
    return index;
}

char* getOperand(char *pASM, int index) {
    char* temp = (char*)malloc(sizeof(char) * 4);
    strncpy(temp, &pASM[index], 3);
    temp[3] = '\0';
    return temp;
}

int getInteger(char *pASM, int index) {
    int i = 0;
    while (pASM[index] != ' ' && pASM[index] != '\t' && pASM[index] != '\0' && pASM[index] != '(') {
        i++;
        index++;
    }
    char* temp = (char*)malloc(sizeof(char) * (i + 1));
    strncpy(temp, &pASM[index - i], i);
    temp[i] = '\0';
    int16_t result = (int16_t)strtol(temp, NULL, 10);
    free(temp);
    return result;
}

ParseResult* initParseResult() {
    ParseResult* pResult = calloc(1, sizeof(ParseResult));
    pResult->Mnemonic = NULL;
    pResult->rdName = NULL;
    pResult->rsName = NULL;
    pResult->rtName = NULL;

    pResult->Imm = 0;
    pResult->rd = 255;
    pResult->rs = 255;
    pResult->rt = 255;

    pResult->Opcode = NULL;
    pResult->Funct = NULL;

    pResult->RD = NULL;
    pResult->RS = NULL;
    pResult->RT = NULL;
    pResult->IMM = NULL;

    return pResult;
}

char* convertSignedDecTo16Bit2sComplement(int value) {
    char* result = (char*)malloc(sizeof(char) * 17);
    result[16] = '\0';
    if (value < 0) {
        value = 65536 + value;
    }
    for (int i = 15; i >= 0; i--) {
        if (value % 2 == 0) {
            result[i] = '0';
        } else {
            result[i] = '1';
        }
        value = value / 2;
    }
    return result;
}

ParseResult* parseASM(const char* const pASM) {
  ParseResult* pResult = initParseResult();

  // Copy ASMInstruction.
  pResult->ASMInstruction = malloc(sizeof(char) * (strlen(pASM) + 1));
  strcpy(pResult->ASMInstruction, pASM);

  // Get mnemonic.
  int i = 0;
  while (pASM[i] != ' ' && pASM[i] != '\t') {
    i++;
  }

  // Copy mnemonic.
  pResult->Mnemonic = malloc(i + 2);
  strncpy(pResult->Mnemonic, pASM, i);

  // Skip whitespace.
  i = skipWhiteSpace(pASM, i);

  if (strcmp(pResult->Mnemonic, "addi") == 0) {
    pResult->rtName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->rsName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->Imm = getInteger(pASM, i);

    Instruction* pInstruction = getInstruction(pResult->Mnemonic);
    pResult->Opcode = pInstruction->opcode;
    pResult->Funct = pInstruction->funct;

    Register* pRegister = getRegister(pResult->rtName);
    pResult->rt = pRegister->decimal;
    pResult->RT = pRegister->binary;

    pRegister = getRegister(pResult->rsName);
    pResult->rs = pRegister->decimal;
    pResult->RS = pRegister->binary;

    pResult->IMM = convertSignedDecTo16Bit2sComplement(pResult->Imm);
  } else if (strcmp(pResult->Mnemonic, "mul") == 0 
          || strcmp(pResult->Mnemonic, "sub") == 0) {
    pResult->rdName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->rsName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->rtName = getOperand(pASM, i);

    Instruction* pInstruction = getInstruction(pResult->Mnemonic);
    pResult->Opcode = pInstruction->opcode;
    pResult->Funct = pInstruction->funct;

    Register* pRegister = getRegister(pResult->rdName);
    pResult->rd = pRegister->decimal;
    pResult->RD = pRegister->binary;

    pRegister = getRegister(pResult->rsName);
    pResult->rs = pRegister->decimal;
    pResult->RS = pRegister->binary;

    pRegister = getRegister(pResult->rtName);
    pResult->rt = pRegister->decimal;
    pResult->RT = pRegister->binary;
  } else if (strcmp(pResult->Mnemonic, "lui") == 0) {
    pResult->rtName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->Imm = getInteger(pASM, i);

    Instruction* pInstruction = getInstruction(pResult->Mnemonic);
    pResult->Opcode = pInstruction->opcode;
    pResult->Funct = pInstruction->funct;

    Register* pRegister = getRegister(pResult->rtName);
    pResult->rt = pRegister->decimal;
    pResult->RT = pRegister->binary;

    pResult->IMM = convertSignedDecTo16Bit2sComplement(pResult->Imm);
  } else if (strcmp(pResult->Mnemonic, "lw") == 0) {
    pResult->rtName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->Imm = getInteger(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->rsName = getOperand(pASM, i);

    Instruction* pInstruction = getInstruction(pResult->Mnemonic);
    pResult->Opcode = pInstruction->opcode;
    pResult->Funct = pInstruction->funct;

    Register* pRegister = getRegister(pResult->rtName);
    pResult->rt = pRegister->decimal;
    pResult->RT = pRegister->binary;

    pRegister = getRegister(pResult->rsName);
    pResult->rs = pRegister->decimal;
    pResult->RS = pRegister->binary;

    pResult->IMM = convertSignedDecTo16Bit2sComplement(pResult->Imm);
  } else {
    pResult->rsName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);
    pResult->rtName = getOperand(pASM, i);
    i = skipWhiteSpace(pASM, i + 4);

    Instruction* pInstruction = getInstruction(pResult->Mnemonic);
    pResult->Opcode = pInstruction->opcode;
    pResult->Funct = pInstruction->funct;

    Register* pRegister = getRegister(pResult->rsName);
    pResult->rs = pRegister->decimal;
    pResult->RS = pRegister->binary;

    pRegister = getRegister(pResult->rtName);
    pResult->rt = pRegister->decimal;
    pResult->RT = pRegister->binary;
  }

  return pResult;
}
