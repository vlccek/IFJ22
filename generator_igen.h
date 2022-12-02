/**
 * @file generator_igen.h
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @brief generator tříadresného kódu
 * @version 0.1
 * @date 2022-10-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef IFJ22_IGEN_H
#define IFJ22_IGEN_H
#include "common.h"
#include "generator_3adres.h"
#include "symtable.h"

void initIgen(i3Table_t program);
void startFunctionCall(token_t *token);
void functionParam(i3Table_t program, token_t *token);

#endif // IFJ22_IGEN_H