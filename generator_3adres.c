/**
 * @file generator_3adres.c
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @brief definice datových typů pro práci s tříadresným kódem
 * @version 0.1
 * @date 2022-10-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generator_3adres.h"


void initializeProgram(i3Table_t *program) {
    program[0]->functionName = "P_MainBody";
    initializeInstructionArray(100, program[0], "F_MainBody");
}

void initializeInstructionArray(int maxCapacity, i3InstructionArray_t *array, char *functionName) {
    make_var(instructionArray, i3Instruction_t *, maxCapacity);
    array->instructions = instructionArray;
    array->capacity = maxCapacity;
    array->functionName = functionName;
    array->size = 0;
}

void pushToArray(i3InstructionArray_t *array, i3Instruction_t instruction) {

    if (array->size >= array->capacity) {
        array->instructions = realloc(array->instructions, sizeof(i3Instruction_t) * array->capacity * 2);
        if (!array->instructions) {
            InternalError("realloc fail!");
        }
        array->capacity *= 2;
    }
    if (array->size < array->capacity) {
        array->instructions[array->size].type = instruction.type;
        array->instructions[array->size].dest = instruction.dest;
        array->instructions[array->size].arg1 = instruction.arg1;
        array->instructions[array->size].arg2 = instruction.arg2;
        array->size++;
    } else {
        InternalError("Not enough space in array to insert a instruction!");
    }
}
