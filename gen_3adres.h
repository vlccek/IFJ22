/**
 * @file gen_3adres.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief definice datových typů pro práci s tříadresným kódem
 * Implementation IFJ22 compiler
 */
#ifndef IFJ22_3ADRES_H
#define IFJ22_3ADRES_H

#include "common.h"
#include "symtable.h"

typedef enum i3InstructionType {
    I_NOOP,
    I_ADDS,
    I_SUBS,
    I_MULS,
    I_DIVS,
    I_IDIVS,
    I_CONCAT,
    I_EQ,
    I_NEQ,
    I_LT,
    I_GT,
    I_LEQ,
    I_GEQ,
    I_AND,
    I_OR,
    I_NOT,
    I_MOVE,
    I_MOVE_PARAM,
    I_MOVE_RETURN,
    I_POPFRAME,
    I_PUSHFRAME,
    I_CREATEFRAME,
    I_INT2FLOAT,
    I_READ,
    I_WRITE,
    I_STRLEN,
    I_LABEL,
    I_JUMP,
    I_JUMP_IF_TRUE,
    I_JUMP_IF_FALSE,
    I_JUMPS_NEQ,
    I_JUMPS_EQ,
    I_LTS,// mensi
    I_GTS,// vetší
    I_EQS, // stejné
    I_CALL,
    I_RETURN,
    I_DEFVAR,
    I_PUSHS,
    I_POPS,
    I_INT2FLOATS,
    I_FLOAT2INTS,
    I_INT2CHARS,
    I_STRI2INTS
} i3InstructionType_t;

/**
 * @brief tříadresná instrukce
 * 
 */
typedef struct i3Instruction {
    i3InstructionType_t type;
    symbol_t dest;
    symbol_t arg1;
    symbol_t arg2;

    bool checkedType;
} i3Instruction_t;

/**
 * @brief seznam tříadresných instrukcí
 * 
 */
typedef struct i3InstructionArray {
    char *functionName;
    i3Instruction_t *instructions;
    size_t size;
    size_t capacity;
} i3InstructionArray_t;

typedef i3InstructionArray_t i3Table_t[MAX_HTSIZE];

void pushToArray(i3InstructionArray_t *array, i3Instruction_t instruction);

void initializeInstructionArray(int maxCapacity, i3InstructionArray_t *array, char *functionName);

void initializeProgram(i3Table_t *program);
char *copyString(char *toCopy);
void insertInstruction(i3InstructionArray_t *array, i3Instruction_t toInsert, size_t pos);
dynStr_t *functionParamInternalName(size_t number);


#endif//IFJ22_3ADRES_H
