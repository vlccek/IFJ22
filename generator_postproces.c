//
// Created by tonda on 07/12/22.
//

#include "generator_postproces.h"
void postProcArray(i3Table_t array, symtable_t *symtable) {
}

void postprocess(i3Table_t program, symtable_t symtable) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        if (&program[i] == NULL)
            return;
        postProcArray(&program[i], &symtable);
    }
}


#include "generator_postproces.h"
