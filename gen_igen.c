/**
 * @file gen_igen.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief Generator of internal three adress code
 * Implementation IFJ22 compiler
 */

#include "gen_igen.h"

typedef struct currentState {
    size_t currentArray;
    size_t lastUsedArray;          // poslední array, do kterého se zapisoval kód funkce
    symbol_t *callingFunction;     // todo: this should be nulled somewhere
    symbol_t undefinedVariable;    // variable is floating in the middle of assignment
    symbol_t newFunction;          // symbol nové funkce, který si pamatujeme
    lexType lastVarType;           // typ proměnné při definici funkce, kterou si pamatujeme
    size_t functionCallParamNumber;// kolikátý parametr funkce právě zpracováme
    genericStack *labelStack;      // stack pro jméne labelů co se pooužívají v ifech a whilech

    // used by expression parsing
    int ifImmersion;
    int elseImmersion;
    bool generateReturn;
} currentState_T;


symtable_t symtable;
currentState_T currentState;

void initIgen(i3Table_t program) {
    symInit(&symtable);
    currentState.undefinedVariable.type = undefinedType;
    currentState.currentArray = 0;
    currentState.lastUsedArray = 0;
    currentState.callingFunction = NULL;
    currentState.functionCallParamNumber = 0;
    currentState.labelStack = ifS_Init();
    currentState.generateReturn = false;
}


void functionDefBegin(char *identifier) {
    symSwitch(&symtable);
    currentState.newFunction = *createSymbolFunction(identifier,
                                                     function,
                                                     NULL,
                                                     undefinedDataType);
}
void functionDefParam(char *identifier, token_t token) {
    insDTList(currentState.newFunction.firstParam,
              tokenTypeToSymbolType(currentState.lastVarType), identifier);
    symbol_t symbol = createSymbolVarLit(identifier, variable,
                                         tokenTypeToSymbolType(currentState.lastVarType), token);
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
        PrettyExit(ERR_FUNCTION_IDENTIFIER);
    }
    symInsertFunction(&symtable, currentState.newFunction);
}

symbol_t *findExistingVariable(char *variableName) {
    symbol_t *symbol = symSearchVar(&symtable, variableName);
    if (symbol == NULL) {
        printlog("Proměnná '%s' nebyla nalezena v tabulce symbolů!\n", variableName);
        PrettyExit(ERR_UNDEFINED_VAR);
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
    symbol_t *symbol = createSymbolFunction(token.data.valueString->string,
                                            function,
                                            NULL,
                                            undefinedDataType);
    symbol->token = token;
    currentState.callingFunction = symbol;
    currentState.functionCallParamNumber = 0;
    if (strcmp(currentState.callingFunction->identifier, "write") != 0) {
        pushFrame(program);
        createFrame(program);
        token_t newtoken = {
                .type = identifierVar,
                .data.valueString = dstrInitChar("$return"),
                .rowNumber = token.rowNumber,
                .rowPosNumber = token.rowPosNumber,
        };
        i3Instruction_t instruction = {
                .type = I_DEFVAR,
                .arg1 = createSymbolVarLit("$return", variable, undefinedDataType, newtoken)};
        pushToArray(&program[currentState.currentArray], instruction);
        token_t niltoken = {
                .type = nullKey,
        };
        i3Instruction_t instnil = {
                .type = I_MOVE,
                .dest = createSymbolVarLit("$return", variable, undefinedDataType, newtoken),
                .arg1 = createSymbolVarLit(NULL, literal, nil, niltoken)};
        pushToArray(&program[currentState.currentArray], instnil);
    }
}
void endFunctionCall(i3Table_t program, token_t token) {
    if (strcmp(currentState.callingFunction->identifier, "write") != 0) {
        i3Instruction_t instruction = {
                .type = I_CALL,
                .arg1 = *currentState.callingFunction};
        pushToArray(&program[currentState.currentArray], instruction);
        if (currentState.undefinedVariable.type == variable) {
            currentState.undefinedVariable.dataType = undefinedDataType;
            symInsert(&symtable, currentState.undefinedVariable);
            functionReturnAssignment(program, currentState.undefinedVariable);
        }
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


void functionParamAssignment(i3Table_t program, symbol_t symbol) {
    i3Instruction_t instruction;
    instruction.type = I_DEFVAR;
    dynStr_t *string = functionParamInternalName(currentState.functionCallParamNumber++);
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

void functionReturnAssignment(i3InstructionArray_t *program, symbol_t symbol) {
    char *identifier = "$return";
    dynStr_t *string = dstrInitChar(identifier);
    token_t token = {
            .type = identifierVar,
            .rowNumber = symbol.token.rowNumber,
            .rowPosNumber = symbol.token.rowPosNumber,
            .data.valueString = string,
    };
    i3Instruction_t instructionMove = {
            .type = I_MOVE_RETURN,
            .dest = symbol,
            .arg1 = createSymbolVarLit(identifier, variable, undefinedDataType, token)};
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
            PrettyExit(ERR_UNDEFINED_VAR);
        }
        newSymbol = *found;
    } else {
        // statement is part of the expression
        newSymbol = createSymbolVarLit(NULL,// todo před odevzdáním tu dej místo null prázdný string.
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
                                                            variable,
                                                            undefinedDataType,
                                                            token);
        i3Instruction_t instruction = {
                .type = I_DEFVAR,
                .arg1 = currentState.undefinedVariable};
        pushToArray(&program[currentState.currentArray], instruction);
    } else {
        currentState.undefinedVariable = *found;
    }
}

void createReturnIns(i3InstructionArray_t *program) {
    i3Instruction_t instruction = {
            .type = I_RETURN,
    };
    pushToArray(&program[currentState.currentArray], instruction);
    currentState.generateReturn = false;
}
/// Complete the command action
void flushCommand(i3Table_t program) {
    if (currentState.undefinedVariable.type != undefinedType && !currentState.callingFunction) {
        createPops(program);
        if (currentState.generateReturn) {
            createReturnIns(program);
        }
    }
    currentState.callingFunction = NULL;
}

void genereateEndOfCondition(i3Table_t program) {
    const char *label = ifS_ending(currentState.labelStack)->string;
    createLabelIns(program, label);
    ifS_old(currentState.labelStack);
}

void exitCodeBlock(i3Table_t program) {
    printSymtable(&symtable);
    if (symDelLocal(&symtable)) {
        // tady vylezl z funkce
        exitFunc();
    } else {
        // tady vylezl z nejakeho zanoreni co neni funkce
        if (ifS_inIfbranch(currentState.labelStack)) {
            // je to konec if bloku
            const char *label = ifS_ending(currentState.labelStack)->string;
            createJumpIns(program, label);
            label = ifS_else(currentState.labelStack)->string;
            createLabelIns(program, label);

            // skočil kladná věte očekávám else
            ifS_SetExpectingElse(currentState.labelStack, true);
            ifS_SetinIfbranch(currentState.labelStack, false);
        } else {
            if (ifs_getType(currentState.labelStack) == while_type) {
                const char *label = ifS_start(currentState.labelStack)->string;
                createJumpIns(program, label);
            }
            genereateEndOfCondition(program);
        }
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
    i3Instruction_t instruction = {
            .type = type,
            .arg1 = {
                    .dataType = undefinedDataType}};
    pushToArray(&program[currentState.currentArray], instruction);
}

void if_creatJumpS(i3Table_t program, i3InstructionType_t type, const char *id) {
    i3Instruction_t instruction = {
            .type = type,
            .arg1.identifier = id,
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

void actionGTS(i3InstructionArray_t *program) {
    createStackInstruction(program, I_GTS);
    const char *label = ifS_else(currentState.labelStack)->string;

    symbol_t data = {
            .type = literal,
            .dataType = booltype,
            .token.data.valueInteger = 1,
    };
    createPushInstruction(program, data);
    if_creatJumpS(program, I_JUMPS_NEQ, label);
}
void actionLTS(i3InstructionArray_t *program) {
    createStackInstruction(program, I_LTS);
    const char *label = ifS_else(currentState.labelStack)->string;

    symbol_t data = {
            .type = literal,
            .dataType = booltype,
            .token.data.valueInteger = 1,
    };
    createPushInstruction(program, data);
    if_creatJumpS(program, I_JUMPS_NEQ, label);
}
void actionEQS(i3InstructionArray_t *program) {
    createStackInstruction(program, I_EQS);
    const char *label = ifS_else(currentState.labelStack)->string;

    symbol_t data = {
            .type = literal,
            .dataType = booltype,
            .token.data.valueInteger = 1,
    };
    createPushInstruction(program, data);
    if_creatJumpS(program, I_JUMPS_NEQ, label);
}
void actionLTSEQ(i3InstructionArray_t *program) {
    // todo
    createStackInstruction(program, I_LTS);


    const char *label = ifS_else(currentState.labelStack)->string;

    symbol_t data = {
            .type = literal,
            .dataType = booltype,
            .token.data.valueInteger = 1,
    };
    createPushInstruction(program, data);
    if_creatJumpS(program, I_JUMPS_NEQ, label);
}
void actionGTSEQ(i3InstructionArray_t *program) {
    // todo
    createStackInstruction(program, I_GTS);
    const char *label = ifS_else(currentState.labelStack)->string;

    symbol_t data = {
            .type = literal,
            .dataType = booltype,
            .token.data.valueInteger = 1,
    };
    createPushInstruction(program, data);
    if_creatJumpS(program, I_JUMPS_NEQ, label);
}

void ifStart() {
    ifS_newIf(currentState.labelStack);
    currentState.ifImmersion++;
    ifS_SetinIfbranch(currentState.labelStack, true);
}
void elseStart() {
    ifS_SetsinElse(currentState.labelStack, true);
    ifS_SetExpectingElse(currentState.labelStack, false);
}

void whilestarts(i3InstructionArray_t *program) {
    ifS_newWhile(currentState.labelStack);
    createLabelIns(program, ifS_start(currentState.labelStack)->string);
}

void checkIfHaveElseBranch(i3InstructionArray_t *program) {
    // no
    if (ifS_expectingElse(currentState.labelStack)) {
        genereateEndOfCondition(program);
    }
}

symbol_t getReturnSymbol() {
    dynStr_t *name = dstrInit();
    dstrAppend(name, "$return");
    symbol_t symbol = {
            .type = variable,
            .dataType = undefinedDataType,
            .identifier = dstrGet(name),
            .token = {
                    .type = stringLiteral,
                    .data.valueString = name,
                    .rowNumber = -1,
                    .rowPosNumber = -1},
    };
    return symbol;
}

void prepareReturn(i3Table_t program) {
    symbol_t symbol = getReturnSymbol();
    currentState.undefinedVariable = symbol;
    currentState.generateReturn = true;
}

void createNewSymtableFrame() {
    symNewLocal(&symtable);
}

void createJumpIns(i3Table_t program, const char *label) {

    i3Instruction_t instruction = {
            .type = I_JUMP,
            .arg1.identifier = label,
    };

    pushToArray(&program[currentState.currentArray], instruction);
}
void createLabelIns(i3Table_t program, const char *label) {

    i3Instruction_t instruction = {
            .type = I_LABEL,
            .arg1.identifier = label,
    };

    pushToArray(&program[currentState.currentArray], instruction);
}

void finalGeneration(i3Table_t program) {
    if (ifS_expectingElse(currentState.labelStack)) {
        genereateEndOfCondition(program);
    }
    postprocess(program, symtable);
    generate(program, symtable);
}
