/**
 * @file expParse.h
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief expression parser
 * Implementace překladače jazyka IFJ22
 */
#ifndef STACK_EXPPARSE_H
#define STACK_EXPPARSE_H

#include "LLtable.h"
#include "common.h"
#include "lex.h"
#include "stack.h"


// for gouping operation in precendece table
typedef enum {
    indexOpPlusMinus = 0,
    indexOpMulDiv,
    indexId,
    indexOpConcat,
    indexLpar,
    indexRpar,
    indexCmp,
    indexDollar,
    indexCount
} precedenceTableIndex;


typedef enum {
    precL = lexTypeCount + 1,// <
    precR,                   //  >
    precEq,                  // =
    precE,                 // nogo
    dollar,
    precendenceTypeCount
} precendenceType;

extern bool isExpInIf;
extern int leftparc;

extern char *precTypeString[];
extern precendenceType precedenceTable[indexCount][indexCount];

rule *derivateTopStack(genericStack *sTokens);

typedef enum {
    exp = precendenceTypeCount + 1
} notTerminaltype;

typedef struct {
    int type;
    token_t *tokenData;
} expParserType;

precedenceTableIndex indexInPrecTable(lexType t);

#define precSymb(x, y) precedenceTable[indexInPrecTable((x)->type)][indexInPrecTable((y)->type)]

// converts precedenc symbol to readeable form
#define precSymbString(a) precTypeString[((int) a % (int) lexTypeCount) - 1]

expParserType *stackTopTerminal(genericStack *s);

unsigned findFirst(genericStack *s, int searchSymb);


// praser for expresions
void expAnal(bool isInIf);

void printExpParserType(void *data);

char *generatePrintExpParsertype(expParserType *data);

unsigned stackTopTerminalIndex(genericStack *s);
#endif//STACK_EXPPARSE_H
