/**
 * @file generator_3adres.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief definice datových typů pro práci s tříadresným kódem
 * Implementace překladače jazyka IFJ22
 */

#include "generator_3adres.h"


void initializeProgram(i3Table_t *program) {
    (*program)[0].functionName = "P_MainBody";
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        (*program)[i].functionName = NULL;
        (*program)[i].instructions = NULL;
        (*program)[i].size = 0;
        (*program)[i].capacity = 0;
    }
    initializeInstructionArray(100, program[0], "F_MainBody");
}

void initializeInstructionArray(int maxCapacity, i3InstructionArray_t *array, char *functionName) {
    make_var(instructionArray, i3Instruction_t*, maxCapacity*sizeof (i3Instruction_t));
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

char* copyString(char* toCopy){
    make_var(newString, char*, strlen(toCopy))
    strcpy(newString, toCopy);
    return newString;
}

symbol_t tokenToSymbol(token_t token, symbolDataType_t type) {
    symbol_t newSymbol;
    newSymbol.rowPosNumber = token.rowPosNumber;
    newSymbol.rowNumber = token.rowNumber;

    if(type == string){
        newSymbol.symbolData.string = copyString(token.data.valueString->string);
    }else if(type == integer){
        newSymbol.symbolData.integer = token.data.valueInteger;
    }else if(type == floating){
        newSymbol.symbolData.floating = token.data.valueFloat;
    }

    newSymbol.type = type;
    if (type == function) {
        newSymbol.identifier = copyString(token.data.valueString->string);
        newSymbol.firstParam = NULL;
        newSymbol.returnType = undefined;
    }else{
        newSymbol.identifier = NULL;
        newSymbol.firstParam = NULL;
        newSymbol.returnType = undefined;

    }
    return newSymbol;
}
