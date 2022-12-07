/**
* @file gen_postproces.c
* @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
* @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
* @brief Post processing of 3 address instructions
* Implementation IFJ22 compiler
*/

#include "gen_postproces.h"
void assignTypeToParams(i3InstructionArray_t *array, symtable_t *symtable, symbol_t *symbol) {
    int len = symbol->firstParam->len;
    DTListMem_T *member = symbol->firstParam->first;
    int i = 0;
    while (member) {
        dynStr_t *dynStr = dstrInitChar(member->identifier);

        token_t tokenDefVar = {
                .type = identifierVar,
                .data.valueString = dynStr,
        };
        symbol_t symb_param = createSymbolVarLit(dstrGet(dynStr),
                                                 variable,
                                                 member->type,
                                                 tokenDefVar);
        i3Instruction_t instruction_defvar = {
                .type = I_DEFVAR,
                .arg1 = symb_param,
        };
        insertInstruction(array, instruction_defvar, 0);
        dynStr_t *name = functionParamInternalName(i++);
        token_t tokenParam = {
                .type = identifierVar,
                .data.valueString = name,
        };
        symbol_t symb_name = createSymbolVarLit(dstrGet(name),
                                                variable,
                                                member->type,
                                                tokenParam);
        i3Instruction_t instruction_move = {
                .type = I_MOVE,
                .dest = symb_param,
                .arg1 = symb_name,
        };
        insertInstruction(array, instruction_move, 1);
        member = member->next;
    }
}

symbol_t *callInsToSymb(i3Instruction_t instruction, symtable_t *symtable) {
    symbol_t *funcSymbol = symSearchFunc(symtable, instruction.arg1.identifier);
    if (funcSymbol == NULL) {
        loging("Calling undefined function:");
        printSymbol(&instruction.arg1);
        printlog("\n");
        PrettyExit(ERR_FUNCTION_IDENTIFIER);
    }
    return funcSymbol;
}

void assignFuncReturnTypes(i3InstructionArray_t *array, symtable_t *symtable) {
    for (int i = 0; i < array->size; ++i) {
        if (array->instructions[i].type == I_MOVE_RETURN) {
            symbol_t *funcSymbol = callInsToSymb(array->instructions[i - 1], symtable);
            array->instructions[i].dest.dataType = funcSymbol->returnType;
        }
    }
}

bool isCompareIns(i3InstructionType_t type) {
    i3InstructionType_t types[] = {
            I_LTS,
            I_GTS,
            I_EQS,
            I_LT_OR_EQ,
            I_GT_OR_EQ,
    };
    size_t n = sizeof(types) / sizeof(i3InstructionType_t);
    for (int i = 0; i < n; ++i) {
        if (types[i] == type) {
            return true;
        }
    }
    return false;
}

bool isStackIns(i3InstructionType_t type) {
    i3InstructionType_t types[] = {

            I_ADDS,
            I_SUBS,
            I_MULS,
            I_DIVS,
            I_IDIVS,
            I_LTS,
            I_GTS,
            I_EQS,
            I_CONCATS,
            I_LT_OR_EQ,
            I_GT_OR_EQ,
    };
    size_t n = sizeof(types) / sizeof(i3InstructionType_t);
    for (int i = 0; i < n; ++i) {
        if (types[i] == type) {
            return true;
        }
    }
    return false;
}

bool isPushOrStackIns(i3InstructionType_t type) {
    return I_PUSHS == type || isStackIns(type);
}


i3Instruction_t *getNextNotCheckedIns(i3InstructionArray_t *array, size_t lookAt) {
    for (int i = (int) lookAt; i >= 0; --i) {
        if (isPushOrStackIns(array->instructions[i].type)) {
            if (array->instructions[i].checkedType == false) {
                array->instructions[i].checkedType = true;
                return &array->instructions[i];
            }
        }
    }
    InternalError("There should definitely be another push instruction");
}

void setCheckedPushToFalse(i3InstructionArray_t *array) {
    for (size_t i = 0; i < array->size; ++i) {
        array->instructions[i].checkedType = false;
    }
}
int whichConvert(symbolDataType_t type1, symbolDataType_t type2) {
    if (type1 == type2) {
        return 0;
    }
    if (type1 == integer && type2 == floating) {
        return 1;
    }
    if (type1 == floating && type2 == integer) {
        return 2;
    }
    return 3;
}

void insertTopToFloat(i3Table_t program, size_t pos) {
    i3Instruction_t intConvertIns = {
            .type = I_INT2FLOATS,
    };
    insertInstruction(program, intConvertIns, pos);
}

void floatConvert(i3InstructionArray_t *array, i3Instruction_t *convertAfter) {
    if (convertAfter->arg1.dataType == floating)
        return;

    size_t insAtPos = convertAfter - array->instructions + 1;
    // add one to insert AFTER this instruction
    insertTopToFloat(array, insAtPos);
    convertAfter->dest.dataType = floating;
}

size_t convToSameType(i3InstructionArray_t *array, i3Instruction_t *i1, i3Instruction_t *i2) {
    int toConvert = whichConvert(i1->arg1.dataType, i2->arg1.dataType);
    if (toConvert > 0) {
        if (toConvert == 1) {
            floatConvert(array, i1);
        } else if (toConvert == 2) {
            floatConvert(array, i2);
        }
        return 1;
    }
    return 0;
};

void findDataTypeIns(i3Instruction_t *ins, size_t lookAt, i3InstructionArray_t *array) {
    for (int i = (int) lookAt; i >= 0; --i) {
        if (array->instructions[i].type == I_MOVE_RETURN) {
            if (!strcmp(ins->arg1.identifier, array->instructions[i].dest.identifier)) {
                ins->arg1.dataType = array->instructions[i].dest.dataType;
                return;
            }
        }
        if (array->instructions[i].type == I_POPS) {
            ins->arg1.dataType = array->instructions[i].dest.dataType;
            return;
        }
    }
}

void findTypes(i3Instruction_t *ins, size_t insAtPos, i3InstructionArray_t *array) {
    if (ins->arg1.dataType == undefinedDataType) {
        findDataTypeIns(ins, insAtPos, array);
    }
}

/// Convert two instructions to same type by adding INT2FLOATS ins
/// \param array Array of instructions
/// \param insAtPos Position of the instruction
/// \return number of instructions added
size_t assignTypeToStackIns(i3InstructionArray_t *array, size_t insAtPos) {
    size_t addedIns = 0;
    i3Instruction_t *i1 = getNextNotCheckedIns(array, insAtPos - 1);
    i3Instruction_t *i2 = getNextNotCheckedIns(array, insAtPos - 1);
    // i2 has to be called first
    findTypes(i2, insAtPos - 1, array);
    findTypes(i1, insAtPos - 1, array);
    if (i1 == NULL || i2 == NULL) {
        InternalError("Wrongly using stack instructions");
    }
    if (array->instructions[insAtPos].type == I_DIVS) {
        addedIns = 2;
        floatConvert(array, i1);
        floatConvert(array, i2);
    } else {
        addedIns = convToSameType(array, i1, i2);
    }

    if (isCompareIns(array->instructions[insAtPos + addedIns].type)) {
        array->instructions[insAtPos + addedIns].arg1.dataType = booltype;
    } else {
        if (addedIns > 0) {
            array->instructions[insAtPos + addedIns].arg1.dataType = floating;
        } else {
            array->instructions[insAtPos + addedIns].arg1.dataType = i1->arg1.dataType;
        }
    }
    array->instructions[insAtPos + addedIns].checkedType = false;
    return addedIns;
}


bool isReturnPop(i3Instruction_t instruction) {
    if (!strcmp(instruction.dest.identifier, "$return"))
        return true;
    return false;
}

bool checkFunctionReturnType(symbolDataType_t dataType1, symbolDataType_t dataType2) {
    bool success = true;
    if (dataType2 == integerNullable) {
        success = dataType1 == integer || dataType1 == integerNullable;

    } else if (dataType2 == floatingNullable) {
        success = dataType1 == floating || dataType1 == floatingNullable;

    } else if (dataType2 == stringNullable) {
        success = dataType1 == string || dataType1 == stringNullable;

    } else {
        success = dataType1 == dataType2;
    }
    return success;
}

void assignTypeToPopsIns(i3InstructionArray_t *array, size_t insAtPos, symtable_t *symtable) {
    i3Instruction_t *i1 = getNextNotCheckedIns(array, insAtPos - 1);
    findTypes(i1, insAtPos - 1, array);
    if (isReturnPop(array->instructions[insAtPos])) {
        symbol_t *funcSymb = symSearchFunc(symtable, array->functionName);
        if (!checkFunctionReturnType(i1->arg1.dataType, funcSymb->returnType)) {
            printSymbol(&i1->arg1);// TODO MBY PRINT DIFFERENT ERROR
            PrintErrorExit("%s", ERR_FUNC_PARAM_RET_TYPE_OR_QUANTITY,
                           "Wrong return type at token above!");
        }
    }
    array->instructions[insAtPos].dest.dataType = i1->arg1.dataType;
}

void convertTypesOnStack(i3InstructionArray_t *array, symtable_t *symtable) {
    setCheckedPushToFalse(array);
    for (size_t i = 0; i < array->size; ++i) {
        if (array->instructions[i].type == I_POPS) {
            assignTypeToPopsIns(array, i, symtable);
        }
        if (isStackIns(array->instructions[i].type)) {
            if (array->instructions[i].arg1.dataType == undefinedDataType) {
                i += assignTypeToStackIns(array, i);
            }
        }
    }
}
void postProcDefvar(i3InstructionArray_t *array) {
    bool isInFuctCall = false;
    htTable_t table;
    htInit(&table);
    for (int i = 0; i < array->size; ++i) {
        if (array->instructions[i].type == I_PUSHFRAME) {
            isInFuctCall = true;
        }
        if (array->instructions[i].type == I_POPFRAME) {
            isInFuctCall = false;
        }
        if (isInFuctCall) {
            continue;
        }
        if (array->instructions[i].type == I_DEFVAR) {
            i3Instruction_t inst = array->instructions[i];
            symbol_t arg1 = inst.arg1;
            htItem_t *item = htSearch(&table, arg1.identifier);
            if (!item) {
                htInsertItem(&table, arg1.token.data.valueString->string, arg1);
                insertInstruction(array, inst, 0);

                i++;
                array->instructions[i].type = I_NOOP;
            } else {
                array->instructions[i].type = I_NOOP;
            }
        }
    }
    htDestroy(&table);
}

i3Instruction_t *findCreateFrameIns(i3Instruction_t *instruction) {
    // there should definitely be CREATEFRAME above
    // this will find and return it by pointer arithmetics
    while (instruction->type != I_CREATEFRAME) {
        instruction--;
    }
    return instruction;
}
void exitBadParamsCall(symbol_t *symb) {
    printSymbol(symb);
    printlog("\n");
    exit(ERR_FUNC_PARAM_RET_TYPE_OR_QUANTITY);
}

void compareParams(struct DTList *paramList, i3Instruction_t *callIns, i3Instruction_t *createFrameIns) {
    size_t expectedCount = countDtList(paramList);
    DTListMem_T *param = paramList->first;
    size_t paramCounter = 0;
    while (callIns != ++createFrameIns) {
        if (createFrameIns->type != I_MOVE_PARAM) {
            continue;
        }
        if (param == NULL) {
            loging("Function params do not match! Expected %zu, got more.", expectedCount);
            exitBadParamsCall(&callIns->arg1);
        }
        if (!checkFunctionReturnType(createFrameIns->arg1.dataType, param->type)) {
            loging("Function param types do not match!");
            exitBadParamsCall(&callIns->arg1);
        }
        param = param->next;
        paramCounter++;
    }
    if (param != NULL) {
        loging("Function params do not match! Got %zu, expected %zu", paramCounter, expectedCount);
        exitBadParamsCall(&callIns->arg1);
    }
}
void assignTypesCallFunc(i3InstructionArray_t *array, symtable_t *symtable) {
    // assign type when call functions
    for (size_t i = 0; i < array->size; ++i) {
        if (array->instructions[i].type == I_CALL) {
            i3Instruction_t *callIns = &array->instructions[i];
            symbol_t *symbol = symSearchFunc(symtable, callIns->arg1.identifier);
            i3Instruction_t *createFrameIns = findCreateFrameIns(&array->instructions[i]);
            compareParams(symbol->firstParam, callIns, createFrameIns);
        }
    }
}


void postProcArray(i3InstructionArray_t *array, symtable_t *symtable) {
    symbol_t *symbol = symSearchFunc(symtable, array->functionName);
    if (!symbol) {
        if (strcmp(array->functionName, "$MainBody") != 0) {
            InternalError("Generuju funkci, co není v symtabulce :(");
        }
    }
    postProcDefvar(array);
    if (symbol) {
        // do not process in main body
        assignTypeToParams(array, symtable, symbol);
    }
    // don't you dare to change order of next calls
    assignFuncReturnTypes(array, symtable);
    assignTypesCallFunc(array, symtable);
    convertTypesOnStack(array, symtable);
}

void postprocess(i3Table_t program, symtable_t symtable) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        if (program[i].functionName == NULL || program[i].size == 0)
            return;
        postProcArray(&program[i], &symtable);
    }
}


#include "gen_postproces.h"
