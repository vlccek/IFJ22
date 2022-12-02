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

void initializeProgram(i3htItem_t *program) {
    program[0].key = "P_MainBody";
    initializeInstructionArray(100, &program[0].array, "F_MainBody");
}

void initializeInstructionArray(int maxCapacity, i3InstructionArray_t *array, char *functionName) {
    make_var(instructionArray, i3Instruction_t *, maxCapacity);
    array->array = instructionArray;
    array->capacity = maxCapacity;
    array->functionName = functionName;
    array->size = 0;
}

void pushToArray(i3InstructionArray_t *array, i3Instruction_t instruction) {
    if (array->size < array->capacity) {
        array->array[array->size].type = instruction.type;
        array->array[array->size].dest = instruction.dest;
        array->array[array->size].arg1 = instruction.arg1;
        array->array[array->size].arg2 = instruction.arg2;
        array->size++;
    } else {
        // todo: realloc and add
        InternalError("Exit not enough space in instruction array!");
    }
}
