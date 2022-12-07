/**
 * @file semanticActionInfo.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @brief Informace pro semantické akce
 * Implementace překladače jazyka IFJ22
 */

#ifndef IFJ22_SEMANTICACTIONINFO_H
#define IFJ22_SEMANTICACTIONINFO_H
#include "lex.h"

typedef enum expressionAction {
    APlus,
    AMinus,
    ADivision,
    AMultiplication,
    AConcatenation,
    AGreaterThen,
    ALowerThen,
    AEq,
    AGreaterThenEq,
    ALowerThenEq,
    ANotAnAction
} expressionAction_t;

typedef struct semanticActionInfo {
    token_t lastToken;
    expressionAction_t action;
} semanticActionInfo;

#endif//IFJ22_SEMANTICACTIONINFO_H
