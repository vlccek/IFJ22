/**
 * @file generator_generator.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief generátor výsledného kódu
 * Implementace překladače jazyka IFJ22
 */

#include "generator_generator.h"

void generateHeader() {
    printf(".IFJcode22\n");
}

const char *convertChar(char c, char convBuffer[51]) {
    sprintf(convBuffer, "\\0%d", c);
    return convBuffer;
}
char *convertString(dynStr_t *dynStr, char *string) {
    char buf[2];
    char convBuffer[51];
    convBuffer[0] = '\0';
    buf[1] = '\0';
    char c;
    while ((c = *string++) != '\0'){
        buf[0] = c;
        if(c < 33 || c == 35 || c == 92)
            dstrAppend(dynStr, convertChar(c, convBuffer));
        else
            dstrAppend(dynStr, buf);
    }
    return dynStr->string;
}
char *generateArgSymbol(symbol_t symb, char *buf) {
    if (symb.type == string){
        dynStr_t* string = dstrInit();
        sprintf(buf, "string@%s", convertString(string, symb.symbolData.string));
        dstrFree(string);
    }
    else if (symb.type == integer)
        sprintf(buf, "int@%d", symb.symbolData.integer);
    else if (symb.type == floating)
        sprintf(buf, "float@%a", symb.symbolData.floating);
    return buf;
}

void generateWrite(i3Instruction_t instruction) {
    char buf[2048];
    printf("WRITE %s", generateArgSymbol(instruction.arg1, buf));
}

void generateInstruction(i3Instruction_t instruction) {
    switch (instruction.type) {
        case I_ADD:
            break;
        case I_SUB:
            break;
        case I_MUL:
            break;
        case I_DIV:
            break;
        case I_IDIV:
            break;
        case I_CONCAT:
            break;
        case I_EQ:
            break;
        case I_NEQ:
            break;
        case I_LT:
            break;
        case I_GT:
            break;
        case I_LEQ:
            break;
        case I_GEQ:
            break;
        case I_AND:
            break;
        case I_OR:
            break;
        case I_NOT:
            break;
        case I_MOVE:
            break;
        case I_INT2FLOAT:
            break;
        case I_READ:
            break;
        case I_WRITE:
            generateWrite(instruction);
            break;
        case I_STRLEN:
            break;
        case I_LABEL:
            break;
        case I_JUMP:
            break;
        case I_JUMP_IF_TRUE:
            break;
        case I_JUMP_IF_FALSE:
            break;
        case I_CALL:
            break;
        case I_RETURN:
            break;
        case I_DEFVAR:
            break;
    }
    printf("\n");
}

void generateInstructionArray(i3InstructionArray_t array) {
    if (array.instructions == NULL)
        return;
    for (int i = 0; i < array.size; ++i) {
        generateInstruction(array.instructions[i]);
    }
    loging("Vygenerovana funkce: %s\n", array.functionName);
}


void generate(i3Table_t program) {
    generateHeader();
    printf("\n");
    printf("CREATEFRAME\nPUSHFRAME\n");
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        generateInstructionArray(program[i]);
    }
}
