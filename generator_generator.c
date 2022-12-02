/**
 * @file generator_generator.c
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @brief generátor výsledného kódu
 * @version 0.1
 * @date 2022-10-21
 * 
 * @copyright Copyright (c) 2022
 * 
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

void generate(i3htItem_t *program) {
    generateHeader();
    if(program[0].array.array[0].type == I_WRITE){
        generateWrite(program[0].array.array[0].dest.identifier);

    }
    loging("Jmeno programu: %s\n", program[0].key);
}
