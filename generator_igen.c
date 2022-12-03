/**
 * @file generator_igen.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief generator tříadresného kódu
 * Implementace překladače jazyka IFJ22
 */

#include "generator_igen.h"

typedef struct currentState {
    size_t currentArray;
    symbol_t *callingFunction; // todo: this should be nulled somewhere
    symbol_t *undefinedVariable;// variable is floating in the middle of assignment
} currentState_T;


symtable_t symtable;
currentState_T currentState;

void initIgen(i3Table_t program) {
    symInit(&symtable);
    currentState.currentArray = 0;
    currentState.callingFunction = NULL;
}

symbol_t *findExistingVariable(char *variableName) {
    symbol_t *symbol = symSearchVar(&symtable, variableName);
    if (symbol == NULL) {
        // todo: exit with right code
        InternalError("Varibale '%s' not found in symtable!", variableName);
    }
    return symbol;
}

void startFunctionCall(token_t token) {
    symbol_t *fceSymbol = symSearchFunc(&symtable, token.data.valueString->string);
    if (fceSymbol == NULL) {
        // todo: exit right value
        InternalError("Function %s not found in symtable!", token.data.valueString->string);
    }
    currentState.callingFunction = fceSymbol;
}


symbolDataType_t tokenTypeToSymbolType(lexType type) {
    switch (type) {
        case stringLiteral:
            return string;
        case integerLiteral:
            return integer;
        case floatLiteral:
            return floating;
        default:
            InternalError("Lex type '%s' is not convertable to symbol type!",
                          getTerminalName(type));
    }
}
void writeLiteral(i3Table_t program, token_t token) {
    symbol_t symbol = tokenToSymbol(token, tokenTypeToSymbolType(token.type));

    i3Instruction_t instruction;
    instruction.type = I_WRITE;
    instruction.arg1 = symbol;
    pushToArray(&program[currentState.currentArray], instruction);
}

void writeSymbol(i3Table_t program, symbol_t *symbol) {
    i3Instruction_t instruction;
    instruction.type = I_WRITE;
    instruction.arg1 = *symbol;
    pushToArray(&program[currentState.currentArray], instruction);
}

void functionParam(i3Table_t program, token_t token) {
    if (!strcmp(currentState.callingFunction->identifier, "write")) {
        if (token.type == identifierVar) {
            symbol_t *symbol = findExistingVariable(token.data.valueString->string);
            writeSymbol(program, symbol);
        } else {
            writeLiteral(program, token);
        }
    }
}

void moveToVariable(i3Table_t program, token_t token) {
    currentState.undefinedVariable->type = tokenTypeToSymbolType(token.type);
    symInsert(&symtable, *currentState.undefinedVariable);

    i3Instruction_t instruction = {
            .type = I_MOVE,
            .dest = *currentState.undefinedVariable,
            .arg1 = tokenToSymbol(token,
                                  tokenTypeToSymbolType(token.type))};
    pushToArray(&program[currentState.currentArray], instruction);
    currentState.undefinedVariable = NULL;
}

void newStatement(i3Table_t program, token_t token) {
    if (currentState.callingFunction != NULL) {
        // if in the middle of function call
        functionParam(program, token);
    } else {
        // statement is part of the expression
        moveToVariable(program, token);
    }
}

void newVariable(i3InstructionArray_t *program, token_t token) {
    symbol_t *symbol = symSearchVar(&symtable, token.data.valueString->string);
    if (symbol != NULL) {
        InternalError("Redefinitions of variable is prohibited!");
        // todo: or is it?
    }
    // todo: symbol does not have all props initialized - mby use tokenToSymbol()
    symbol = createSymbol(token.data.valueString->string,
                          undefined, // we do not know variable type by now
                          NULL,      // variable does not have param list
                          undefined);// variable does not have return value

    currentState.undefinedVariable = symbol;

    i3Instruction_t instruction = {
            .type = I_DEFVAR,
            .arg1 = *symbol};
    pushToArray(&program[currentState.currentArray], instruction);
}
