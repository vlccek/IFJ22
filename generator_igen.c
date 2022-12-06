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
    size_t lastUsedArray;          // poslední array, do kterého se zapisoval kód funkce
    symbol_t *callingFunction;     // todo: this should be nulled somewhere
    symbol_t undefinedVariable;    // variable is floating in the middle of assignment
    symbol_t newFunction;          // symbol nové funkce, který si pamatujeme
    lexType lastVarType;           // typ proměnné při definici funkce, kterou si pamatujeme
    size_t functionCallParamNumber;// kolikátý parametr funkce právě zpracováme

    // used by expression parsing
    int immersion;
} currentState_T;


symtable_t symtable;
currentState_T currentState;

void initIgen(i3Table_t program) {
    symInit(&symtable);
    currentState.currentArray = 0;
    currentState.lastUsedArray = 0;
    currentState.callingFunction = NULL;
    currentState.functionCallParamNumber = 0;
}


void functionDefBegin(char *identifier) {
    symSwitch(&symtable);
    currentState.newFunction = *createSymbolFunction(identifier, function, NULL, undefinedDataType);
}
void functionDefParam(char *identifier, token_t token) {
    insDTList(currentState.newFunction.firstParam, tokenTypeToSymbolType(currentState.lastVarType), identifier);
    symbol_t symbol = createSymbolVarLit(identifier, variable, tokenTypeToSymbolType(currentState.lastVarType), token);
    symInsert(&symtable, symbol);
}
void functionDefParamRememberType(lexType type) {
    currentState.lastVarType = type;
}
void functionDefRet(token_t token) {
    currentState.newFunction.returnType = tokenTypeToSymbolType(token.type);
    symbol_t *search = symSearchFunc(&symtable, currentState.newFunction.identifier);
    if (search) {
        printlog("Pokus o redefinici funkce '%s'\n", currentState.newFunction.identifier);
        PrettyExit(ERR_IDENTIFIER_NAME);
    }
    symInsertFunction(&symtable, currentState.newFunction);
}

symbol_t *findExistingVariable(char *variableName) {
    symbol_t *symbol = symSearchVar(&symtable, variableName);
    if (symbol == NULL) {
        printlog("Proměnná '%s' nebyla nalezena v tabulce symbolů!\n", variableName);
        PrettyExit(ERR_IDENTIFIER_NAME);
    }
    return symbol;
}
void pushFrame(i3Table_t program) {
    i3Instruction_t instruction;
    instruction.type = I_PUSHFRAME;
    pushToArray(&program[currentState.currentArray], instruction);
}
void popFrame(i3Table_t program) {
    i3Instruction_t instruction;
    instruction.type = I_POPFRAME;
    pushToArray(&program[currentState.currentArray], instruction);
}
void createFrame(i3Table_t program) {
    i3Instruction_t instruction;
    instruction.type = I_CREATEFRAME;
    pushToArray(&program[currentState.currentArray], instruction);
}

void startFunctionCall(i3Table_t program, token_t token) {
    // todo: a nakonec parsování kontrola podle reálného záznamu
    symbol_t *symbol = createSymbolFunction(token.data.valueString->string, function, NULL, undefinedDataType);
    currentState.callingFunction = symbol;
    currentState.functionCallParamNumber = 0;
    if (strcmp(currentState.callingFunction->identifier, "write") != 0) {
        pushFrame(program);
        createFrame(program);
    }
}
void endFunctionCall(i3Table_t program, token_t token) {
    //todo konec volani funkce
    loging("konec volani funkce");
    if (strcmp(currentState.callingFunction->identifier, "write") != 0) {
        popFrame(program);
    }
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
            return undefinedDataType;
        default:
            InternalError("Lex type '%s' is not convertable to symbol type!",
                          getTerminalName(type));
    }
}

void writeSymbol(i3Table_t program, symbol_t symbol) {
    i3Instruction_t instruction;
    instruction.type = I_WRITE;
    instruction.arg1 = symbol;
    pushToArray(&program[currentState.currentArray], instruction);
}
dynStr_t *functionParamInternalName() {
    dynStr_t *string = dstrInit();
    dstrAppend(string, "$param");
    char buf[128];
    sprintf(buf, "%zu", currentState.functionCallParamNumber++);
    dstrAppend(string, buf);
    return string;
}

void functionParamAssignment(i3Table_t program, symbol_t symbol) {
    i3Instruction_t instruction;
    instruction.type = I_DEFVAR;
    dynStr_t *string = functionParamInternalName();
    char *identifier = dstrGet(string);
    token_t token = symbol.token;
    token.data.valueString = string;
    instruction.arg1 = createSymbolVarLit(identifier, variable, undefinedDataType, token);
    pushToArray(&program[currentState.currentArray], instruction);


    i3Instruction_t instructionMove = {
            .type = I_MOVE_PARAM,
            .dest = instruction.arg1,
            .arg1 = symbol};
    pushToArray(&program[currentState.currentArray], instructionMove);
}

void functionParam(i3Table_t program, token_t token) {
    if (!strcmp(currentState.callingFunction->identifier, "write")) {
        if (token.type == identifierVar) {
            symbol_t *symbol = findExistingVariable(token.data.valueString->string);
            writeSymbol(program, *symbol);
        } else {
            symbol_t newSymbol = createSymbolVarLit("",
                                                    literal,
                                                    tokenTypeToSymbolType(token.type),
                                                    token);
            writeSymbol(program, newSymbol);
        }
    } else {
        if (token.type == identifierVar) {
            symbol_t *symbol = findExistingVariable(token.data.valueString->string);
            functionParamAssignment(program, *symbol);
        } else {
            symbol_t newSymbol = createSymbolVarLit("",
                                                    literal,
                                                    tokenTypeToSymbolType(token.type),
                                                    token);
            functionParamAssignment(program, newSymbol);
        }
    }
}

void createPops(i3Table_t program) {
    currentState.undefinedVariable.dataType = undefinedDataType;
    symInsert(&symtable, currentState.undefinedVariable);

    i3Instruction_t instruction = {
            .type = I_POPS,
            .dest = currentState.undefinedVariable};
    pushToArray(&program[currentState.currentArray], instruction);
    currentState.undefinedVariable.type = undefinedType;
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

symbol_t tokenToSymbol(token_t token) {
    symbol_t newSymbol;
    if (token.type == identifierVar) {
        symbol_t *found = findExistingVariable(token.data.valueString->string);
        if (!found) {
            printlog("Chyba: proměnná $%s nenalezena ", token.data.valueString->string);
            PrettyExit(ERR_IDENTIFIER_NAME);
        }
        newSymbol = *found;
    } else {
        // statement is part of the expression
        newSymbol = createSymbolVarLit("",
                                       literal,
                                       tokenTypeToSymbolType(token.type),
                                       token);
    }
    return newSymbol;
}

void createPushInstruction(i3Table_t program, symbol_t symbol) {
    i3Instruction_t instruction = {
            .type = I_PUSHS,
            .arg1 = symbol,
    };
    pushToArray(&program[currentState.currentArray], instruction);
}

void newStatement(i3Table_t program, token_t token) {
    if (currentState.callingFunction != NULL) {
        // if in the middle of function call
        functionParam(program, token);
    } else {
        symbol_t newSymbol = tokenToSymbol(token);
        createPushInstruction(program, newSymbol);
    }
}

void newVariable(i3InstructionArray_t *program, token_t token) {
    if (token.type != identifierVar)
        InternalError("Not a variable identifier");
    symbol_t *found;
    if ((found = symSearchVar(&symtable, token.data.valueString->string)) == NULL) {
        currentState.undefinedVariable = createSymbolVarLit(token.data.valueString->string,
                                                            variable,         // we do not know variable type by now
                                                            undefinedDataType,// variable does not have param list
                                                            token);           // variable does not have return value
        i3Instruction_t instruction = {
                .type = I_DEFVAR,
                .arg1 = currentState.undefinedVariable};
        pushToArray(&program[currentState.currentArray], instruction);
    } else {
        currentState.undefinedVariable = *found;
    }
}

/// Complete the command action
void flushCommand(i3Table_t program) {
    if (currentState.undefinedVariable.type != undefinedType) {
        createPops(program);
    }
    currentState.callingFunction = NULL;
}

void exitCodeBlock() {
    printSymtable(&symtable);
    if (symDelLocal(&symtable)) {
        exitFunc();
    }
}

void enterFunc(i3InstructionArray_t *program, char *identifier) {
    currentState.currentArray = ++currentState.lastUsedArray;
    if (currentState.currentArray >= MAX_HTSIZE) {
        InternalError("Nelze vytvořit další pole pro zápis tříadresných instrukcí.");
    }
    initializeInstructionArray(100, &program[currentState.currentArray], copyString(identifier));
}

void exitFunc() {
    currentState.currentArray = 0;
}

void createStackInstruction(i3Table_t program, i3InstructionType_t type) {
    if (type != I_ADDS &&
        type != I_DIVS &&
        type != I_IDIVS &&
        type != I_MULS &&
        type != I_SUBS)
        InternalError("Creating stack instruction but not actually!");
    i3Instruction_t instruction = {
            .type = type,
    };
    pushToArray(&program[currentState.currentArray], instruction);
}

void actionPlus(i3Table_t program) {
    createStackInstruction(program, I_ADDS);
}
void actionSubtraction(i3Table_t program) {
    createStackInstruction(program, I_SUBS);
}
void actionMultiplication(i3Table_t program) {
    createStackInstruction(program, I_MULS);
}
void actionConcat(i3Table_t program) {
    createStackInstruction(program, I_MULS);
}
void actionDivision(i3Table_t program) {
    createStackInstruction(program, I_DIVS);
}

void actionGT(i3InstructionArray_t *program) {
    createStackInstruction(program, I_GTS);
}

void ifstart() {
    currentState.immersion++;
}