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
    symbol_t undefinedVariable;// variable is floating in the middle of assignment
    symbol_t newFunction;      // symbol nové funkce, který si pamatujeme
    lexType lastVarType;       // typ proměnné při definici funkce, kterou si pamatujeme

    // used by expression parsing
    symbol_t tmp1;// primary temporary symbol
    symbol_t tmp2;// secondary temporary symbol
} currentState_T;


symtable_t symtable;
currentState_T currentState;

void initIgen(i3Table_t program) {
    symInit(&symtable);
    currentState.currentArray = 0;
    currentState.callingFunction = NULL;
    currentState.tmp1.type = undefinedType;
    currentState.tmp2.type = undefinedType;
}


void functionDefBegin(char *identifier) {
    symSwitch(&symtable);
    currentState.newFunction = *createSymbolFunction(identifier, function, NULL, undefined);
}
void functionDefParam(char *identifier) {
    insDTList(currentState.newFunction.firstParam, tokenTypeToSymbolType(currentState.lastVarType), identifier);
}
void functionDefParamRememberType(lexType type) {
    currentState.lastVarType = type;
}
void functionDefRet(token_t token) {
    currentState.newFunction.returnType = tokenTypeToSymbolType(token.type);
    symbol_t *search = symSearchFunc(&symtable, currentState.newFunction.identifier);
    if (search)
    {
        printlog("Pokus o redefinici funkce '%s'\n", currentState.newFunction.identifier);
        PrettyExit(ERR_IDENTIFIER_NAME);
    }
    symInsertFunction(&symtable, currentState.newFunction);
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
    // todo: vložit název funkce a počet parametrů do nějaké tabulky, pozdější kontroly
    // todo: a nakonec parsování kontrola podle reálného záznamu
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
        case stringKey:
            return string;
        case integerLiteral:
        case intKey:
            return integer;
        case floatLiteral:
        case floatKey:
            return floating;
        case nullKey:
            return nil;
        case stringNullKey:
            return stringNullable;
        case intNullKey:
            return integerNullable;
        case floatNullKey:
            return floatingNullable;
        case voidKey:
            return undefined;
        default:
            InternalError("Lex type '%s' is not convertable to symbol type!",
                          getTerminalName(type));
    }
}
void writeLiteral(i3Table_t program, token_t token) {
    symbol_t symbol;
    ;// = tokenToSymbol(token, tokenTypeToSymbolType(token.type));

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

void moveToVariable(i3Table_t program, symbol_t symbol) {
    currentState.undefinedVariable.dataType = symbol.dataType;
    symInsert(&symtable, currentState.undefinedVariable);

    i3Instruction_t instruction = {
            .type = I_MOVE,
            .dest = currentState.undefinedVariable,
            .arg1 = symbol};
    pushToArray(&program[currentState.currentArray], instruction);
}

void newStatement(i3Table_t program, token_t token) {
    if (currentState.callingFunction != NULL) {
        // if in the middle of function call
        functionParam(program, token);
    } else {
        // statement is part of the expression
        symbol_t newSymbol = createSymbolVarLit("",
                                                literal,
                                                tokenTypeToSymbolType(token.type),
                                                token);
        if (currentState.tmp1.type == undefinedType) {
            currentState.tmp1 = newSymbol;
        } else if (currentState.tmp2.type == undefinedType) {
            currentState.tmp2 = newSymbol;
        } else {
            InternalError("No more tmp variables available!");
        }
    }
}

void newVariable(i3InstructionArray_t *program, token_t token) {
    if (token.type != identifierVar)
        InternalError("Not a variable identifier");
    if (!symSearchVar(&symtable, token.data.valueString->string)) {
        // todo: find and assign to undefined
    }
    // todo: symbol does not have all props initialized - mby use tokenToSymbol()
    currentState.undefinedVariable = createSymbolVarLit(token.data.valueString->string,
                                                        variable,         // we do not know variable type by now
                                                        undefinedDataType,// variable does not have param list
                                                        token);           // variable does not have return value

    i3Instruction_t instruction = {
            .type = I_DEFVAR,
            .arg1 = currentState.undefinedVariable};
    pushToArray(&program[currentState.currentArray], instruction);
}

/// Complete the command action
void flushCommand(i3Table_t program) {
    if (currentState.tmp1.type != undefinedType){
        moveToVariable(program, currentState.tmp1);
        currentState.tmp1.type = undefinedType;
    }
}
