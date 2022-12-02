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

void generateInstruction(i3Instruction_t instruction) {

}

void generateInstructionArray(i3InstructionArray_t array){
    if(array.instructions == NULL)
        return;
    for (int i = 0; i < array.size; ++i) {
        generateInstruction(array.instructions[i]);
    }
    loging("Vygenerovana funkce: %s\n", array.functionName);
}


void generate(i3Table_t program) {
    generateHeader();
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        generateInstructionArray(program[i]);
    }
}
