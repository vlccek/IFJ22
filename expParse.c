//
// Created by jvlk on 8.10.22.
//
#include "expParse.h"


precendenceType precedenceTable[indexCount][indexCount] = {
        //a vstup// +- | */ | ID lit... | . | lpar | rpar  | dollar
        {precR, precR, precL, precR, precL, precR, precR},          // +-             //// top b
        {precR, precR, precL, precR, precL, precR, precR},          // */
        {precR, precR, precErr, precErr, precErr, precR, precR},    // ID LIT
        {precErr, precErr, precErr, precErr, precL, precErr, precR},// .
        {precR, precR, precL, precR, precR, precEq, precR},         // lpar
        {precR, precR, precL, precR, precErr, precR, precR},        // rpar
        {precL, precL, precL, precL, precL, precL, precErr}         // dollar
};


char *precTypeString[] = {"precL",
                          "precR",
                          "precEq",
                          "precErr",
                          "dollar",
                          "precendenceTypeCount",
                          "exp"};

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
        case semicolon:
            return indexDollar;
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
    *b = nextToken(NULL);
    make_var(expAnalt, expParserType *, sizeof(expParserType));
    expAnalt->type = b->type;
    expAnalt->tokenData = b;
    return expAnalt;
}

char *generatePrintExpParsertype(expParserType *data) {
    loging("Generatiing Print for stack: ");
    if (data->type < lexTypeCount) {
        loging("Token is : %s", getTerminalName(data->type));
        return getTerminalName(data->type);
    } else {
        char *out = precSymbString(data->type);
        loging("Token: %s", precSymbString(data->type));
        return out;
    }
}


void printExpParserType(void *data) {
    fprintf(stdout, "%s", generatePrintExpParsertype((expParserType *) data));
}

void expAnal() {
    genericStack *sTokens = gStackInit();
    pushPrecedencToken(sTokens, dollar);
    expParserType *b = getTokenP(), *a;
    rule *ruleNum;// ouput var for


    do {
        gStackPrint(sTokens, printExpParserType);
        a = stackTopTerminal(sTokens);
        loging("IN a (Top notnerminal): %s", generatePrintExpParsertype(a));
        loging("IN B: %s (input)", generatePrintExpParsertype(b));
        loging("precedence sympol: %s", precSymbString(precSymb(a, b)));
        switch (precSymb(a, b)) {
            case precR:
                ruleNum = derivateTopStack(sTokens);
                pushExpNonTerminal(sTokens);
                // najde se první < pak se přejde
                break;
            case precL:
                // gStackPush to stack shift symbol before front(<)
                pushPrecedencToken(sTokens, precL);
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
    expParserType *tmp = (expParserType *) gStackGetNth(s, i);
    while (tmp->type > lexTypeCount && tmp->type != dollar) {
        tmp = (expParserType *) gStackGetNth(s, ++i);
        if (tmp == NULL) {
            InternalError("No Terminal in stack in exp-parser :(");
        }
    }
    return gStackGetNth(s, i);
}


unsigned findFirst(genericStack *s, int searchSymb) {
    unsigned i = 0;
    while (((expParserType *) gStackGetNth(s, i++))->type != searchSymb) {//empty
        if (gStackGetNth(s, i) == NULL) {
            InternalError("No Terminal in stack in exp-parser :(");
        }
    }
    return i;
}

rule *derivateTopStack(genericStack *sTokens) {
    expParserType *tmp;
    PSAStackMember *handle[MAX_RULE_LEN];

    unsigned indexOfPrec = findFirst(sTokens, precL) - 1;// for not use the precL
    handle[indexOfPrec] = NULL;                          // setting len of rule
    while (indexOfPrec != 0) {
        tmp = gStackPop(sTokens);
        handle[--indexOfPrec] = createPSAStackMember(tmp->type, nonTerminal);// todo exp je v pici
    }
    free(gStackPop(sTokens));// pop

    rule *r;
    if ((r = findRuleByHandle(handle)) == NULL)
        //todo exit semntika
        exit(ERR_SYNTAX);
    else
        return r;
}
