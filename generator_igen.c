/**
 * @file generator_igen.c
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @brief generator tříadresného kódu
 * @version 0.1
 * @date 2022-10-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generator_igen.h"

typedef struct currentState{
    symbol_t *callingFunction;
    size_t currentArray;
}currentState_T;


symtable_t symtable;
currentState_T currentState;

void initIgen(i3Table_t program){
    symInit(&symtable);
    currentState.currentArray = 0;
    currentState.callingFunction = NULL;
}

void startFunctionCall(token_t *token){
    symbol_t *fceSymbol = symSFunction(&symtable, token->data.valueString->string);
    if (fceSymbol == NULL){
        // todo: exit right value
        InternalError("Function %s not found in symtable!", token->data.valueString->string);
    }
    currentState.callingFunction = fceSymbol;
}


void writeString(i3Table_t program, token_t *token){
    symbol_t stringSymbol;
    stringSymbol.identifier = token->data.valueString->string;
    stringSymbol.rowPosNumber = token->rowPosNumber;
    stringSymbol.rowNumber = token->rowNumber;
    stringSymbol.type = string;

    i3Instruction_t instruction;
    instruction.type = I_WRITE;
    instruction.dest = stringSymbol;
    pushToArray(&program[currentState.currentArray], instruction);
}


void functionParam(i3Table_t program, token_t *token){
    if(!strcmp(currentState.callingFunction->identifier, "write")){
        if(token->type == stringLiteral)
            writeString(program, token);
    }

}