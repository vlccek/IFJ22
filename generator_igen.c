/**
 * @file generator_igen.c
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @brief generator tříadresného kódu
 * @version 0.1
 * @date 2022-10-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generator_igen.h"

symtable_t table;

void initIgen(i3htTable_t program){
    symInit(&table);
    make_var(mainBodyKey, char*, 10);
    mainBodyKey = "P_MainBody";
    program[0].key = mainBodyKey;
}

void initFunctionCall(){

}