/**
 * @file parser.c
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @brief Hlavičkový soubor pro parser jazyka
 * Implementace překladače imperativního jazyka IFJ22
 *
 */

#ifndef STACK_PARSER_H
#define STACK_PARSER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "expParse.h"
#include "lex.h"
#include "stack.h"
#include "symtable.h"


#define MAX_STACK_VIEWABLE 100
typedef struct uniqueCounter {
    int conditionCount;
    int whileCount;
    int forCount;
    int repUntilCount;
    int functionCount;
    int untilCount;
} uniqueCounter_t;


typedef enum expectType{
    fceExpect,
    prttExpect,
    varDecExpect,
    nothingSpecial
}expectType_t;

typedef enum {
    identifSt,
    fceParamsSt,
    fceReturnsSt
}state_t;

typedef struct parserMemory{
    genericStack* PSAStack;
    char* stackView[MAX_STACK_VIEWABLE];
    // here goes other structures
} ParserMemory;

typedef struct fceExpect{
    expectType_t whatToExpect;
    state_t currentState;
}fceExpect_t;

int parser();


#endif //STACK_PARSER_H
