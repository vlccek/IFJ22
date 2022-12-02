/**
 * @file generator_generator.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief generátor výsledného kódu
 * Implementace překladače jazyka IFJ22
 */

#include "generator_generator.h"

void generateHeader(){
    printf(".IFJcode22\n");
}

void generateWrite(const char *value){
    printf("CREATEFRAME\n"
           "DEFVAR TF@%%1\n"
           "MOVE TF@%%1 string@%s\n"
           "WRITE TF@%%1", value);
}

void generate(i3Table_t *program) {
    generateHeader();
    if(program[0]->instructions[0].type == I_WRITE){
        generateWrite(program[0]->instructions[0].dest.identifier);

    }
    loging("Jmeno programu: %s\n", program[0]->functionName);
}
