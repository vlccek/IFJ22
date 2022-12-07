//
// Created by tonda on 07/12/22.
//

#include "generator_postproces.h"
#include "generator_3adres.h"
void postProcArrayFunctionParams(i3InstructionArray_t *array, symtable_t *symtable, symbol_t *symbol) {
    int len = symbol->firstParam->len;
    printf("# len: %d\n", len);
    DTListMem_T *member = symbol->firstParam->first;
    while (member) {

        member = member->next;
    }
    // todo vygenerovat $param1 -> $reálný identifikátor
}

symbol_t *callInsToSymb(i3Instruction_t instruction, symtable_t *symtable) {
    symbol_t *funcSymbol = symSearchFunc(symtable, instruction.arg1.identifier);
    if (funcSymbol == NULL) {
        printSymbol(&instruction.arg1);
        loging("Calling undefined function at!\n");
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
