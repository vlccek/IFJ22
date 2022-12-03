/**
 * @file generator_3adres.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief definice datových typů pro práci s tříadresným kódem
 * Implementace překladače jazyka IFJ22
 */
#ifndef IFJ22_3ADRES_H
#define IFJ22_3ADRES_H

#include "common.h"
#include "symtable.h"

typedef enum i3InstructionType {
    I_ADD,
    I_SUB,
    I_MUL,
    I_DIV,
    I_IDIV,
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
    I_INT2FLOAT,
    I_READ,
    I_WRITE,
    I_STRLEN,
    I_LABEL,
    I_JUMP,
    I_JUMP_IF_TRUE,
    I_JUMP_IF_FALSE,
    I_CALL,
    I_RETURN,
    I_DEFVAR 
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
#endif //IFJ22_3ADRES_H
