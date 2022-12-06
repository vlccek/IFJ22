/**
 * @file generator_igen.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief generator tříadresného kódu
 * Implementace překladače jazyka IFJ22
 */

#ifndef IFJ22_IGEN_H
#define IFJ22_IGEN_H
#include "common.h"
#include "generator_3adres.h"
#include "symtable.h"

void initIgen(i3Table_t program);
void startFunctionCall(token_t token);
symbolDataType_t tokenTypeToSymbolType(lexType type);
void newStatement(i3Table_t program, token_t token);
void writeLiteral(i3Table_t program, token_t token);
void newVariable(i3Table_t program, token_t token);
void functionDefParamRememberType(lexType type);
void functionDefParam(char *identifier);
void functionDefBegin(char *identifier);
void functionDefRet(token_t token);
void flushCommand(i3Table_t program);
void actionPlus(i3Table_t program);
void exitCodeBlock();
void enterFunc(i3InstructionArray_t *program, char *identifier);
void exitFunc();


#endif// IFJ22_IGEN_H