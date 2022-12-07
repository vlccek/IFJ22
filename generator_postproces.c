//
// Created by tonda on 07/12/22.
//

#include "generator_postproces.h"
#include "generator_3adres.h"
void postProcArrayFunctionParams(i3InstructionArray_t *array, symtable_t *symtable, symbol_t *symbol) {
    if (strcmp(array->functionName, "$MainBody") == 0) {
        return;
    }
    int len = symbol->firstParam->len;
    printf("# len: %d\n", len);
    DTListMem_T *member = symbol->firstParam->first;
    while (member) {

        member = member->next;
    }
    // todo vygenerovat $param1 -> $reálný identifikátor
}
void postProcArray(i3InstructionArray_t *array, symtable_t *symtable) {
    symbol_t *symbol = symSearchFunc(symtable, array->functionName);
    if (!symbol) {
        if (strcmp(array->functionName, "$MainBody") != 0) {
            InternalError("Generuju funkci, co není v symtabulce :(");
        }
    }
    postProcArrayFunctionParams(array, symtable, symbol);
}

void postprocess(i3Table_t program, symtable_t symtable) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        if (&program[i] == NULL)
            return;
        postProcArray(&program[i], &symtable);
    }
}


#include "generator_postproces.h"
