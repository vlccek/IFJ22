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

#include "lex.h"
#include "symtable.h"
#include "common.h"
#include "stack.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef TESTING
token *testTokens;
#endif

#ifdef TESTING
token getTokenTesting(FILE *fp) {
    return *testTokens++;
}
token (*nextToken)(FILE *fp) = getTokenTesting;
#else
token_t (*nextToken)(FILE *fp) = getToken;
#endif

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

typedef enum state{
    identifSt,
    fceParamsSt,
    fceReturnsSt
}state_t;

typedef struct parserMemory{
    genericStack* PSAStack;
    // here goes other structures
} ParserMemory;

typedef struct fceExpect{
    expectType_t whatToExpect;
    state_t currentState;
}fceExpect_t;

int parser();


#endif //STACK_PARSER_H
