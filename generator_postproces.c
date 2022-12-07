//
// Created by tonda on 07/12/22.
//

#include "generator_postproces.h"
void postProcArrayFunctionParams(i3InstructionArray_t *array, symtable_t *symtable, symbol_t *symbol) {
    int len = symbol->firstParam->len;
    DTListMem_T *member = symbol->firstParam->first;
    int i = 0;
    while (member) {
        dynStr_t *dynStr = dstrInitChar(member->identifier);

        token_t tokenDefVar = {
                .type = identifierVar,
                .data.valueString = dynStr,
        };
        symbol_t symb_param = createSymbolVarLit(dstrGet(dynStr), variable, member->type, tokenDefVar);
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
        symbol_t symb_name = createSymbolVarLit(dstrGet(name), variable, member->type, tokenParam);
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

void postProcArray(i3InstructionArray_t *array, symtable_t *symtable) {
    // todo: nechceme double DEFVAR instrukce - nastavit na NOOP
    // todo: vyřeš typové konverze
    symbol_t *symbol = symSearchFunc(symtable, array->functionName);
    if (!symbol) {
        if (strcmp(array->functionName, "$MainBody") != 0) {
            InternalError("Generuju funkci, co není v symtabulce :(");
        }
    }
    if (symbol) {
        // do not process in main body
        postProcArrayFunctionParams(array, symtable, symbol);
    }
    assignFuncReturnTypes(array, symtable);
}

void postprocess(i3Table_t program, symtable_t symtable) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        if (program[i].functionName == NULL || program[i].size == 0)
            return;
        postProcArray(&program[i], &symtable);
    }
}


#include "generator_postproces.h"
