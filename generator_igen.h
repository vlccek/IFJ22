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
void startFunctionCall(i3Table_t program, token_t token);
void endFunctionCall(i3Table_t program, token_t token);
symbolDataType_t tokenTypeToSymbolType(lexType type);
void newStatement(i3Table_t program, token_t token);
void writeLiteral(i3Table_t program, token_t token);
void newVariable(i3Table_t program, token_t token);
void functionDefParamRememberType(lexType type);
void functionDefParam(char *identifier, token_t token);
void functionDefBegin(char *identifier);
void functionDefRet(token_t token);
void flushCommand(i3Table_t program);
void actionPlus(i3Table_t program);
void actionMultiplication(i3Table_t program);
void actionDivision(i3Table_t program);
void actionSubtraction(i3Table_t program);
void actionConcat(i3Table_t program);
void actionGTS(i3InstructionArray_t *program);
void actionLTS(i3InstructionArray_t *program);
void actionEQS(i3InstructionArray_t *program);
void actionLTSEQ(i3InstructionArray_t *program); // >=
void actionGTSEQ(i3InstructionArray_t *program); // <=
void ifStart();

void exitCodeBlock();
void enterFunc(i3InstructionArray_t *program, char *identifier);
void exitFunc();
void prepareReturn(i3Table_t program);


#endif// IFJ22_IGEN_H