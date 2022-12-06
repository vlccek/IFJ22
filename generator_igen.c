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
    size_t lastUsedArray;      // poslední array, do kterého se zapisoval kód funkce
    symbol_t *callingFunction; // todo: this should be nulled somewhere
    symbol_t undefinedVariable;// variable is floating in the middle of assignment
    symbol_t newFunction;      // symbol nové funkce, který si pamatujeme
    lexType lastVarType;       // typ proměnné při definici funkce, kterou si pamatujeme

    // used by expression parsing
    symbol_t tmp1;// primary temporary symbol
    symbol_t tmp2;// secondary temporary symbol
    symbolDataType_t lastDataTypeOnStack;
} currentState_T;


symtable_t symtable;
currentState_T currentState;

void initIgen(i3Table_t program) {
    symInit(&symtable);
    currentState.currentArray = 0;
    currentState.lastUsedArray = 0;
    currentState.callingFunction = NULL;
    currentState.tmp1.type = undefinedType;
    currentState.tmp2.type = undefinedType;
    currentState.lastDataTypeOnStack = undefinedDataType;
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
        // todo: exit with right code
        printlog("Varibale '%s' not found in symtable!\n", variableName);
        PrettyExit(ERR_IDENTIFIER_NAME);
    }
    return symbol;
}

void startFunctionCall(token_t token) {
    // todo: vložit název funkce a počet parametrů do nějaké tabulky, pozdější kontroly
    // todo: a nakonec parsování kontrola podle reálného záznamu
    symbol_t *symbol = createSymbolFunction(token.data.valueString->string, function, NULL, undefinedDataType);
    currentState.callingFunction = symbol;
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
        // todo
    }
}

void createPops(i3Table_t program) {
    currentState.undefinedVariable.dataType = currentState.lastDataTypeOnStack;
    currentState.lastDataTypeOnStack = undefinedDataType;
    symInsert(&symtable, currentState.undefinedVariable);

    i3Instruction_t instruction = {
            .type = I_POPS,
            .dest = currentState.undefinedVariable};
    pushToArray(&program[currentState.currentArray], instruction);
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

void newStatement(i3Table_t program, token_t token) {
    if (currentState.callingFunction != NULL) {
        // if in the middle of function call
        functionParam(program, token);
    } else {
        symbol_t newSymbol = tokenToSymbol(token);

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
    if (currentState.tmp1.type != undefinedType) {
        moveToVariable(program, currentState.tmp1);
        currentState.tmp1.type = undefinedType;
    } else {
        if (currentState.lastDataTypeOnStack != undefinedDataType)
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

void createPushInstruction(i3Table_t program, symbol_t symbol) {
    i3Instruction_t instruction = {
            .type = I_PUSHS,
            .arg1 = symbol,
    };
    pushToArray(&program[currentState.currentArray], instruction);
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

void action(i3Table_t program, i3InstructionType_t type) {
    if (currentState.tmp1.type == undefinedType)
        InternalError("There is nothing to add/div/...!");

    // push another
    createPushInstruction(program, currentState.tmp1);
    currentState.lastDataTypeOnStack = currentState.tmp1.dataType;
    currentState.tmp1.type = undefinedType;

    if (currentState.tmp2.type != undefinedType) {
        createPushInstruction(program, currentState.tmp2);
        currentState.tmp2.type = undefinedType;
    }

    createStackInstruction(program, type);
}

void convertTypes(symbolDataType_t convertTo, symbol_t *toBeConverted) {
    if (toBeConverted->dataType == convertTo)
        return;

    if (convertTo == floating && toBeConverted->dataType == integer) {
        toBeConverted->dataType = floating;
        toBeConverted->token.data.valueFloat = (float) toBeConverted->token.data.valueInteger;
        return;
    }

    InternalError("Cannot convert!\n");
}

void convertTopToFloat(i3Table_t program) {
    if (currentState.lastDataTypeOnStack != integer)// todo: probably aritmetic error
        InternalError("Cannot convert anything else than int");
    i3Instruction_t instruction = {
            .type = I_INT2FLOATS,
    };
    pushToArray(&program[currentState.currentArray], instruction);
}
void prepareOperandTypes(i3Table_t program, bool convToFloat) {
    // if one of them if floating, then both of them has to be floating
    if (currentState.tmp1.dataType == floating ||
        currentState.tmp2.dataType == floating) {
        convertTypes(floating, &currentState.tmp1);
        convertTypes(floating, &currentState.tmp2);
    } else if (convToFloat) {// if division
        convertTypes(floating, &currentState.tmp1);
        convertTypes(floating, &currentState.tmp2);
    }
    if (currentState.lastDataTypeOnStack != undefinedDataType) {
        if (currentState.tmp1.dataType == floating)
            convertTopToFloat(program);
    }
}


void actionPlus(i3Table_t program) {
    prepareOperandTypes(program, false);
    action(program, I_ADDS);
}
void actionSubtraction(i3Table_t program) {
    prepareOperandTypes(program, false);
    action(program, I_SUBS);
}
void actionMultiplication(i3Table_t program) {
    prepareOperandTypes(program, false);
    action(program, I_MULS);
}
void actionConcat(i3Table_t program) {
    prepareOperandTypes(program, false);
    action(program, I_MULS);
}
void actionDivision(i3Table_t program) {
    // todo: covert both to float
    prepareOperandTypes(program, true);
    action(program, I_DIVS);
}
