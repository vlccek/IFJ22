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
    /*
    if(addedIns == 0){
        // if same types, then final have this type
        array->instructions[insAtPos + addedIns].arg1.dataType = i1->arg1.dataType;
    } else {
        // if not same type, than surely converted to float
        // todo if not bool type
        array->instructions[insAtPos + addedIns].arg1.dataType = floating;
    }
     */
    array->instructions[insAtPos + addedIns].checkedType = false;
    return addedIns;
}

void assignTypeToPopsIns(i3InstructionArray_t *array, size_t insAtPos) {
    i3Instruction_t *i1 = getNextNotCheckedIns(array, insAtPos - 1);
    // findTypes(i1, insAtPos - 1, array);
    array->instructions[insAtPos].dest.dataType = i1->arg1.dataType;
}
void convertTypesOnStack(i3InstructionArray_t *array) {
    setCheckedPushToFalse(array);
    for (size_t i = 0; i < array->size; ++i) {
        if (array->instructions[i].type == I_POPS) {
            assignTypeToPopsIns(array, i);
        }
        if (isStackIns(array->instructions[i].type)) {
            if (array->instructions[i].arg1.dataType == undefinedDataType) {
                i += assignTypeToStackIns(array, i);
            }
        }
    }
}

void postProcArray(i3InstructionArray_t *array, symtable_t *symtable) {
    // todo: nechceme double DEFVAR instrukce - nastavit na NOOP
    symbol_t *symbol = symSearchFunc(symtable, array->functionName);
    if (!symbol) {
        if (strcmp(array->functionName, "$MainBody") != 0) {
            InternalError("Generuju funkci, co není v symtabulce :(");
        }
    }
    if (symbol) {
        // do not process in main body
        assignTypeToParams(array, symtable, symbol);
    }
    assignFuncReturnTypes(array, symtable);
    convertTypesOnStack(array);
    // todo: return types and calling errors
}

void postprocess(i3Table_t program, symtable_t symtable) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        if (program[i].functionName == NULL || program[i].size == 0)
            return;
        postProcArray(&program[i], &symtable);
    }
}


#include "gen_postproces.h"
