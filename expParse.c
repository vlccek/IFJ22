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
    gStackPush(sTokens, nPrec);
}
void pushExpNonTerminal(genericStack *sTokens) {
    make_var(nPrec, expParserType *, sizeof(token_t));
    nPrec->type = exp;
    nPrec->tokenData = NULL;
    gStackPush(sTokens, nPrec);
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


void expAnal() {
    genericStack *sTokens = gStackInit();
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
                // gStackPush to stack shift symbol before front(<)
                tmp = gStackPop(sTokens);
                pushPrecedencToken(sTokens, precL);
                gStackPush(sTokens, tmp);
                gStackPush(sTokens, b);
                b = getTokenP();
                break;
            case precEq:
                gStackPush(sTokens, a);
                b = getTokenP();
                break;
            case precErr:
            default:
                InternalError("Neznáma, nebo nedovolená kombinace tokenů");
                break;
        }
    } while (true);
}

expParserType *stackTopTerminal(genericStack *s) {
    unsigned i = 0;
    while (((expParserType *) gStackGetNth(s, i++))->type < lexTypeCount) {//empty
    }
    return gStackGetNth(s, i);
}


unsigned findFirst(genericStack *s, int searchSymb) {
    unsigned i = 0;
    while (((expParserType *) gStackGetNth(s, i++))->type != searchSymb) {//empty
    }
    return i;
}

rule *derivateTopStack(genericStack *sTokens) {
    expParserType *tmp;
    PSAStackMember *handle[MAX_RULE_LEN];

    unsigned indexOfPrec = findFirst(sTokens, precR);
    while (indexOfPrec != 0) {
        tmp = gStackPop(sTokens);
        handle[indexOfPrec--] = createPSAStackMember(tmp->type, nonTerminal);
    }

    rule *r;
    if ((r = findRuleByHandle(handle)) == NULL)
        //todo exit semntika
        exit(ERR_SYNTAX);
    else
        return r;
}
