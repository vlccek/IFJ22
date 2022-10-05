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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef struct uniqueCounter{
    int conditionCount;
    int whileCount;
    int forCount;
    int repUntilCount;
    int functionCount;
    int untilCount;
}uniqueCounter_t;

typedef enum typeOnTop{
    endOfFile,
    terminal,
    nonTerminal,
    fail
}typeOnTop;

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

typedef struct fceExpect{
    expectType_t whatToExpect;
    state_t currentState;
}fceExpect_t;

int testParserTokens(token *tokens);

int parser(int argc, char *argv[]);


#endif //STACK_PARSER_H
