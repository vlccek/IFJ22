//
// Created by jvlk on 8.10.22.
//

#include "expParse.h"


precedenceTableIndex indexInPrecTable(lexType t) {
    switch (t) {
        case plusOp:
        case minusOp:
            return indexOpPlusMinus;
            break;
        case divisionOp:
        case multiplicationOp:
            return indexOpMulDiv;
            break;
        case concatenationOp:
            return indexOpConcat;
            break;
        case identifierVar:
        case identifierFunc:
        case stringLiteral:
        case floatLiteral:
        case integerLiteral:
            return indexId;
            break;
        case leftPar:
            return indexLpar;
        case rightPar:
            return indexRpar;
        case dollar:
            return dollar;
            break;
    }
}


/**
 * Push precedentTyp to stack
 * @param precT
 */
void pushPrecedencToken(genericStack *sTokens, precendenceType precT) {
    make_var(nPrec, expParserType *, sizeof(token_t));
    nPrec->type = (lexType) (int) precT;
    nPrec->tokenData = NULL;
    push(sTokens, nPrec);
}
void pushExpNonTerminal(genericStack *sTokens) {
    make_var(nPrec, expParserType *, sizeof(token_t));
    nPrec->type = exp;
    nPrec->tokenData = NULL;
    push(sTokens, nPrec);
}

/**
 * Use get token function and allocate memory for it. Wraps to expPraserType.
 * @return
 */
expParserType *getTokenP() {
    make_var(b, token_t *, sizeof(token_t));
    *b = getToken(NULL);
    make_var(expAnalt, expParserType *, sizeof(expParserType));
    expAnalt->type = b->type;
    expAnalt->tokenData = b;
    return expAnalt;
}


expParserType *stackTopTerminal(genericStack *s) {
    expParserType *tmp, *rvalue;
    genericStack *s2 = stackInit();
    while ((tmp = pop(s))->type < lexTypeCount ) {
        push(s2, tmp);
    }
    rvalue = stackTop(s2);

    while (sIsEmpty(s2)) {
        push(s, pop(s2));
    }
    return rvalue;
}

void expAnal() {
    genericStack *sTokens = stackInit();
    pushPrecedencToken(sTokens, dollar);
    expParserType *b = getTokenP(), *a;
    expParserType *tmp;//
    rule *ruleNum;     // ouput var for


    do {
        a = stackTopTerminal(sTokens);
        switch (precSymb(a, b)) {
            case precR:
                ruleNum = derivateTopStack(sTokens);
                pushExpNonTerminal(sTokens);
                // najde se první < pak se přejde
                break;
            case precL:
                // push to stack shift symbol before front(<)
                tmp = pop(sTokens);
                pushPrecedencToken(sTokens, precL);
                push(sTokens, tmp);
                push(sTokens, b);
                b = getTokenP();
                break;
            case precEq:
                push(sTokens, a);
                b = getTokenP();
                break;
            case precErr:
            default:
                InternalError("Neznáma, nebo nedovolená kombinace tokenů");
                break;
        }
    } while (true);
}
rule *derivateTopStack(genericStack *sTokens) {
    int i = 0;
    expParserType *tmp;
    PSAStackMember *handleBuffer[MAX_RULE_LEN];
    while ((tmp = pop(sTokens))->type != precR) {
        make_var(tmpHandle, PSAStackMember *, sizeof(PSAStackMember));
        tmpHandle->data = tmp->type;
        tmpHandle->type = nonTerminal;
        handleBuffer[i++] = tmpHandle;
    }

    PSAStackMember *handleOut[MAX_RULE_LEN];
    for (int l = 0; l == i; i > 0) {
        handleOut[l++] = handleBuffer[--i];
    }
    rule *r;
    if ((r = findRuleByHandle(handleOut)) == NULL)
        //todo exit semntika
        exit(ERR_SYNTAX);
    else
        return r;
}
