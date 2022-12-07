/**
 * @file generator_igen.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief Generator of internal three adress code
 * Implementation IFJ22 compiler
 */

#ifndef IFJ22_IGEN_H
#define IFJ22_IGEN_H
#include "common.h"
#include "generator_3adres.h"
#include "generator_generator.h"
#include "generator_postproces.h"
#include "stackIf.h"
#include "symtable.h"

void initIgen(i3Table_t program);
void startFunctionCall(i3Table_t program, token_t token);
void endFunctionCall(i3Table_t program, token_t token);
void functionReturnAssignment(i3Table_t program, symbol_t symbol);
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
void actionLTSEQ(i3InstructionArray_t *program);// >=
void actionGTSEQ(i3InstructionArray_t *program);// <=
void ifStart();
void elseStart();
void whilestarts(i3InstructionArray_t *program);
void checkIfHaveElseBranch(i3InstructionArray_t *program);
void createNewSymtableFrame();

void createJumpIns(i3Table_t program, const char *label);
void createLabelIns(i3Table_t program, const char *label);

void exitCodeBlock(i3Table_t program);
void enterFunc(i3InstructionArray_t *program, char *identifier);
void exitFunc();
void prepareReturn(i3Table_t program);
void finalGeneration(i3Table_t program);

#endif// IFJ22_IGEN_H
