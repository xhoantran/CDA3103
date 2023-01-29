typedef struct _Instruction {
    char* mnemonic;
    char* opcode;
    char* funct;
} Instruction;
Instruction* getInstruction(char* name);

typedef struct _Register {
    char* name;
    int decimal;
    char* binary;
} Register;
Register* getRegister(char* name);
