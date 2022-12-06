/**
 * @file expParse.c
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief expression parser
 * Implementace překladače jazyka IFJ22
 */
#include "expParse.h"


precendenceType precedenceTable[indexCount][indexCount] = {
        //a
        // +- | */ | ID lit... | . | lpar | rpar  | boolOP |dollar
        {precR, precR, precL, precR, precL, precR, precL,  precR}, // +-             //// top b
        {precR, precR, precL, precR, precL, precR, precL, precR}, // */
        {precR, precR, precE, precR, precE, precR, precR, precR}, // ID LIT
        {precE, precE, precL, precE, precL, precE, precL, precR}, // .
        {precR, precR, precL, precR, precR, precEq, precL, precR},// lpar
        {precR, precR, precL, precR, precE, precR, precL, precR}, // rpar
        {precL, precL, precL, precL, precL, precL, precL, precL},// boolOP |
        {precL, precL, precL, precL, precL, precL, precL, precE}  // dollar
};


char *precTypeString[] = {"precL",
                          "precR",
                          "precEq",
                          "precE",
                          "dollar",
                          "precendenceTypeCount",
                          "exp"};

precedenceTableIndex indexInPrecTable(lexType t) {
    switch (t) {
        case plusOp:
        case minusOp:
        case concatenationOp:
            loging("Index in precedenc table: %d", indexOpPlusMinus);
            return indexOpPlusMinus;
            break;
        case divisionOp:
        case multiplicationOp:
            loging("Index in precedenc table: %d", indexOpMulDiv);
            return indexOpMulDiv;
            break;
        case identifierVar:
        case identifierFunc:
        case stringLiteral:
        case floatLiteral:
        case integerLiteral:
            loging("Index in precedenc table: %d", indexId);
            return indexId;
            break;
        case leftPar:
            loging("Index in precedenc table: %d", indexLpar);
            return indexLpar;
        case rightPar:
            loging("Index in precedenc table: %d", indexRpar);
            return indexRpar;
        case dollar:
        case semicolon:
        case ending:
            loging("Index in precedenc table: %d", indexDollar);
            return indexDollar;
            break;
        case lesserEqOp:
        case lesserThanOp:
        case greaterThanOp:
        case greaterEqOp:
        case eqOp:
        case notEqOp:
            loging("Index in precedenc is comapring (bool) symbol for, index : %d", indexCmp);
            return indexCmp;
            break;
        default:
            loging("Index in precedenc table not foung: %d", t);
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
    *b = getToken(stdin);
    make_var(expAnalt, expParserType *, sizeof(expParserType));
    expAnalt->type = b->type;
    expAnalt->tokenData = b;
    return expAnalt;
}

char *generatePrintExpParsertype(expParserType *data) {
    // loging("Generatiing Print for stack: ");
    if (data->type < lexTypeCount) {
        // loging("Token is : %s", getTerminalName(data->type));
        return getTerminalName(data->type);
    } else {
        char *out = precSymbString(data->type);
        // loging("Token: %s", precSymbString(data->type));
        return out;
    }
}

bool precOver(genericStack *s) {
    if (s->c == 1) {
        if (((expParserType *) gStackTop(s))->type == dollar) {
            return false;
        }
    }
    return true;
}

void printExpParserType(void *data) {
    fprintf(stderr, "%s", generatePrintExpParsertype((expParserType *) data));
}

void addPrecLBefore(genericStack *s, unsigned position) {
    genericStack *tmpStack = gStackInit();
    for (int i = (int) position; i > 0; i--) {
        gStackPush(tmpStack, gStackPop(s));
    }

    pushPrecedencToken(s, precL);

    while (!gStackIsEmpty(tmpStack)) {
        gStackPush(s, gStackPop(tmpStack));
    }
}

void expAnal() {
    // )   createLLTable();// todo: remove if not testing
    genericStack *sTokens = gStackInit();
    pushPrecedencToken(sTokens, dollar);
    expParserType *b = getTokenP(), *a;
    rule *ruleNum;// ouput var for


    do {
        gStackPrint(sTokens, printExpParserType);
        a = stackTopTerminal(sTokens);
        loging("IN a (Top notnerminal): %s", generatePrintExpParsertype(a));
        loging("IN B (input): %s ", generatePrintExpParsertype(b));
        loging("precedence sympol: %s", precSymbString(precSymb(a, b)));
        switch (precSymb(a, b)) {
            case precR:
                loging("Entering precR case")
                ruleNum = derivateTopStack(sTokens);
                pushExpNonTerminal(sTokens);
                // najde se první < pak se přejde
                break;
            case precL:
                // gStackPush to stack shift symbol before front(<)
                loging("Entering precL case")
                addPrecLBefore(sTokens, stackTopTerminalIndex(sTokens));
                gStackPush(sTokens, b);
                b = getTokenP();
                break;
            case precEq:
                gStackPush(sTokens, a);
                b = getTokenP();
                break;
            case precE:
            default:
                loging("Not know token, returning to parser") return;
                break;
        }
    } while (precOver(sTokens));
}

expParserType *stackTopTerminal(genericStack *s) {
    return gStackGetNth(s, stackTopTerminalIndex(s));
}

unsigned stackTopTerminalIndex(genericStack *s) {
    unsigned i = 0;
    expParserType *tmp = (expParserType *) gStackGetNth(s, i);
    while (tmp->type > lexTypeCount && tmp->type != dollar) {
        tmp = (expParserType *) gStackGetNth(s, ++i);
        if (tmp == NULL) {
            InternalError("No Terminal in stack in exp-parser :(");
        }
    }
    return i;
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


expressionAction_t convertToAction(lexType data) {
    switch (data) {
        case plusOp:
            return APlus;
        case minusOp:
            return AMinus;
        case divisionOp:
            return ADivision;
        case multiplicationOp:
            return AMultiplication;
        case concatenationOp:
            return AConcatenation;
        default:
            return ANotAnAction;
    }
}
rule *derivateTopStack(genericStack *sTokens) {
    loging("Entering derivate top Ofstack");
    expParserType *tmp;
    PSAStackMember *handle[MAX_RULE_LEN];

    unsigned indexOfPrec = findFirst(sTokens, precL) - 1;// for not use the precL
    handle[indexOfPrec] = NULL;                          // setting len of rule
    while (indexOfPrec != 0) {
        tmp = gStackPop(sTokens);
        if (tmp->type == exp) {
            handle[--indexOfPrec] = createPSAStackMember(Exp, nonTerminal);// hovno kod ale couž:)
        } else {
            handle[--indexOfPrec] = createPSAStackMember(tmp->type, terminal);
        }
    }
    free(gStackPop(sTokens));// pop
    loging("END derivate top Ofstack");

    rule *r;
    if ((r = findRuleByHandle(handle)) == NULL) {
        //todo exit semntika
        loging("Nebylo nalezeno pravidlo :(");
        PrettyExit(ERR_SYNTAX);
    } else {
        semanticActionInfo a;
        if (tmp->tokenData == NULL) {
            // todo: might be broken if ()
            if (handle[1]->type != terminal)
                InternalError("This should definitely be terminal!");
            a.action = convertToAction(handle[1]->data);
        } else {
            a.lastToken = *tmp->tokenData;
            a.action = ANotAnAction;
        }
        r->semanticAction(a);
        loging("END derivate top Ofstack");
        return r;
    }
}
