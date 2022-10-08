//
// Created by jvlk on 8.10.22.
//

#ifndef STACK_EXPPARSE_H
#define STACK_EXPPARSE_H

#include "common.h"
#include "lex.h"
#include "stack.h"

// for gouping operation in precendece table
typedef enum {
    indexId = 0,
    indexOpPlusMinus,
    indexOpMulDiv,
    indexOpConcat,
    indexLpar,
    indexRpar,
    indexDollar,
    indexCount
} precedenceTableIndex;


typedef enum {
    precL = lexTypeCount + 1,// <
    precR,                   //  >
    precEq,                  // =
    precErr,                 // nogo
    dollar
} precendenceType;

precendenceType precedenceTable[indexCount][indexCount] = {
        // +- | */ | ID lit... | lpar | rpar | | dollar
        {precR, precR, precL, precR, precL, precR, precR},        // +-
        {precR, precR, precL, precR, precL, precR, precR},        // */
        {precR, precR, precErr, precErr, precErr, precR},         // ID LIT
        {precErr, precErr, precL, precErr, precL, precErr, precR},// ID LIT
        {precR, precR, precL, precR, precR, precEq, precR},       // lpar
        {precR, precR, precL, precR, precR, precR, precR},        // rpar
        {precL, precL, precL, precL, precL, precL, precL}         // dollar

};
precedenceTableIndex indexInPrecTable(lexType t);

#define precSymb(x, y) precedenceTable[indexInPrecTable(x->type)][indexInPrecTable(y.type)]

genericStack *sTokens;// stack for tokens

// praser for expresions
void expAnal();

#endif//STACK_EXPPARSE_H
