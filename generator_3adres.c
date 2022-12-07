/**
 * @file generator_3adres.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief definice datových typů pro práci s tříadresným kódem
 * Implementace překladače jazyka IFJ22
 */

#include "generator_3adres.h"


void initializeProgram(i3Table_t *program) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        (*program)[i].functionName = NULL;
        (*program)[i].instructions = NULL;
        (*program)[i].size = 0;
        (*program)[i].capacity = 0;
    }
    initializeInstructionArray(100, program[0], "$MainBody");
}

void initializeInstructionArray(int maxCapacity, i3InstructionArray_t *array, char *functionName) {
    make_var(instructionArray, i3Instruction_t *, maxCapacity * sizeof(i3Instruction_t));
    array->instructions = instructionArray;
    array->capacity = maxCapacity;
    array->functionName = functionName;
    array->size = 0;
}

void pushToArray(i3InstructionArray_t *array, i3Instruction_t instruction) {
    if (array->size >= array->capacity) {
        array->instructions = realloc(array->instructions,
                                      sizeof(i3Instruction_t) * array->capacity * 2);
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

char *copyString(char *toCopy) {
    make_var(newString, char *, sizeof(char) * (strlen(toCopy) + 1))
            strcpy(newString, toCopy);
    return newString;
}

void insertInstruction(i3InstructionArray_t *array, i3Instruction_t toInsert, size_t pos) {
    pushToArray(array, toInsert);
    size_t copyCount = array->size - pos - 1;
    memmove(&array->instructions[pos + 1], &array->instructions[pos],
            sizeof(i3Instruction_t) * copyCount);
    array->instructions[pos] = toInsert;
}

dynStr_t *functionParamInternalName(size_t number) {
    dynStr_t *string = dstrInit();
    dstrAppend(string, "$param");
    char buf[128];
    sprintf(buf, "%zu", number);
    dstrAppend(string, buf);
    return string;
}
