/**
 * @file LLTable.c
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @brief Tabulka ktera obsahuje vsechny pravidla
 * Implementace překladače imperativního jazyka IFJ21
 *
 */
#include "LLtable.h"

table Table;

tableMember *getLLMember(nonTerminalType nonterm, lexType terminal) {
    if(nonterm >= nonTerminalCount || terminal >= lexTypeCount)
        InternalError("Trying to access Table[%d][%d]\n"
                      "Max nonTerminal is %d and max terminal: %d ",
                      nonterm, terminal, nonTerminalCount, lexTypeCount);
    return Table[nonterm][terminal];
}

bool stackMemberCmp(PSAStackMember *memberA, PSAStackMember *memberB) {
    return (memberA->type == memberB->type && memberA->data == memberB->data);
}

int cmpRules(tableMember *tMember, PSAStackMember **rightSideOfRule) {
    for (int i = 0; i < MAX_RULES_IN_CELL; ++i) {
        for (int j = 0; j < MAX_RULE_LEN; ++j) {
            if (tMember->rules[i] == NULL) {
                break;
            }
            PSAStackMember *m = rightSideOfRule[j];
            if (tMember->rules[i]->to[j] == NULL && m == NULL) {
                if (j == 0) {
                    break;
                }
                return j;
            }
            if (tMember->rules[i]->to[j] == NULL || m == NULL) {
                break;
            }
            if (!stackMemberCmp(tMember->rules[i]->to[j], m)) {
                break;
            }
        }
    }
    return 0;
}

rule *findRuleByHandle(PSAStackMember *handleToFind[MAX_RULE_LEN]) {
    tableMember *tableMember;
    int ruleIndex;
    for (int token = 0; token < lexTypeCount; ++token) {
        if ((tableMember = Table[Exp][token]) == NULL) {
            continue;
        };
        if ((ruleIndex = cmpRules(tableMember, handleToFind)) != 0)
            return tableMember->rules[ruleIndex] ;
    }
    return NULL;
}

//endregion
//region Table Creation
PSAStackMember *createPSAStackMember(int value, PSADataType type) {
    make_var(member, PSAStackMember *, sizeof (PSAStackMember));
    member->data = value;
    member->type = type;
    return member;
}


tableMember *getMember(int terminal, int nonTerminal) {
    tableMember *member = (tableMember *) malloc(sizeof(tableMember));
    checkNullPointer(member);

    member->nonTerminal = nonTerminal;
    member->terminal = terminal;
    return member;
}


void InserRules(int terminal, int nonTerminal, int memberCount, va_list members, int ruleIndex) {
    rule *r = (rule *) malloc(sizeof(rule));
    checkNullPointer(r);
    static int counter = 0;
    r->id = counter++;
    r->from = nonTerminal;
    r->epsRule = memberCount == 0;
    Table[nonTerminal][terminal]->rules[ruleIndex] = r;

    if (r->epsRule == false) {
        int i;
        for (i = 0; i < MAX_RULES_IN_CELL; ++i) {
            if (i < memberCount) {
                AddToRightSide(terminal, nonTerminal, i, va_arg(members, PSAStackMember * ), ruleIndex);
            } else {
                AddToRightSide(terminal, nonTerminal, i, NULL, ruleIndex);

            }
            // Table[nonTerminal][terminal]->rules[ruleIndex]->to[i] = ((extendedStackMember *) members);
        }
        if (i < MAX_RULE_LEN) {
            // Poslední se dává na null jen pokud jsem nenaplnil celý
            AddToRightSide(terminal, nonTerminal, i, NULL, ruleIndex);
        }
        va_end(members);
    }
}

void insertMember(int terminal, int nonTerminal, int memberCount, ...) {
    va_list members;
    va_start(members, memberCount);

    if (memberCount > MAX_RULE_LEN) {
        InternalError("Trying to insert more rules than possible. (Low MAX_RULE_LEN).");
    }

    if (Table[nonTerminal][terminal] != NULL) {
        if (Table[nonTerminal][terminal]->rules[0] == NULL) // this should never happen
            InternalError("No rules in initialized member!");

        int i;
        for (i = 1; i < MAX_RULES_IN_CELL; ++i) {
            if (Table[nonTerminal][terminal]->rules[i] == NULL) {
                InserRules(terminal, nonTerminal, memberCount, members, i);
                break;
            }
        }
        if (i == MAX_RULES_IN_CELL)
            InternalError("Rules memory overflow.");

    } else {
        tableMember *member = getMember(terminal, nonTerminal);

        Table[nonTerminal][terminal] = member;
        int ruleIndex = 0;
        InserRules(terminal, nonTerminal, memberCount, members, ruleIndex);
        va_end(members);
    }
}

/*
void printTable () {
    if (debug) {
        int radekIndex = 0, sloupecIndex = 0;
        while (radekIndex < 20) {
            fprintf(stderr, "Line: %d\n", radekIndex);
            while (Table[radekIndex] != NULL && Table[radekIndex][sloupecIndex] != NULL) {
                fprintf(stderr, "   Nonterminal type (ze kterého derivuju): `%d` \n",
                        Table[radekIndex][sloupecIndex]->rules.from);
                int i = 0;
                while (Table[radekIndex][sloupecIndex]->rules.to[i] != NULL && i < MAX_RULE_LEN) {
                    // vypíše typdat,value
                    fprintf(stderr, "\t\t%d;%d ", Table[radekIndex][sloupecIndex]->rules.to[i]->typeOfData,
                            Table[radekIndex][sloupecIndex]->rules.to[i]->value);
                    i++;
                }
                fprintf(stderr, " derivuje na `%d` hodnot\n", i);
                sloupecIndex++;
            }
            radekIndex++;
        }
    }
}
*/
PSADataType getDataType(char *name) {
    for (int i = 0; i < nonTerminalCount; ++i) {
        if (!strcmp(name, getNonTerminalName(i)))
            return nonTerminal;
    }
    return terminal;
}

char* getStringPSAMember(PSAStackMember m){
    if(m.type == terminal)
        return getTerminalName(m.data);
    if(m.type == nonTerminal)
        return getNonTerminalName(m.data);
    return "EndOfProgram";
}

//endregion
//region Inserts to table
void createLLTable() {
    static int onlyOneCallAllowed = 0;
    if (onlyOneCallAllowed++ > 0)
        return;

    insertMember(ending, ProgramBody , 0);
    insertMember(semicolon, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(functionKey, ProgramBody , 2,
                 partOfRulesRightSide(FceDefine),
                 partOfRulesRightSide(ProgramBody));
    insertMember(identifierFunc, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(leftPar, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(identifierVar, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(floatLiteral, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(stringLiteral, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(integerLiteral, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(nullKey, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(ifKey, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(whileKey, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(returnKey, ProgramBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(semicolon, Command , 1,
                 partOfRulesRightSide(semicolon));
    insertMember(identifierFunc, Command , 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(leftPar, Command , 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(identifierVar, Command , 2,
                 partOfRulesRightSide(DeclareVariable),
                 partOfRulesRightSide(Command));
    insertMember(floatLiteral, Command , 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(stringLiteral, Command , 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(integerLiteral, Command , 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(nullKey, Command , 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(ifKey, Command , 1,
                 partOfRulesRightSide(Condition));
    insertMember(whileKey, Command , 1,
                 partOfRulesRightSide(While));
    insertMember(returnKey, Command , 1,
                 partOfRulesRightSide(Return));
    insertMember(functionKey, FceDefine , 4,
                 partOfRulesRightSide(FceHeader),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight));
    insertMember(functionKey, FceHeader , 7,
                 partOfRulesRightSide(functionKey),
                 partOfRulesRightSide(identifierFunc),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(FunctionDeclareParams),
                 partOfRulesRightSide(rightPar),
                 partOfRulesRightSide(colon),
                 partOfRulesRightSide(FuncReturnColonType));
    insertMember(rightPar, FunctionDeclareParams , 0);
    insertMember(stringNullKey, FunctionDeclareParams , 2,
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(floatNullKey, FunctionDeclareParams , 2,
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(intNullKey, FunctionDeclareParams , 2,
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(stringKey, FunctionDeclareParams , 2,
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(floatKey, FunctionDeclareParams , 2,
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(intKey, FunctionDeclareParams , 2,
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(rightPar, CommaOrEpsParams , 0);
    insertMember(comma, CommaOrEpsParams , 3,
                 partOfRulesRightSide(comma),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(stringNullKey, DeclareParam , 2,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(identifierVar));
    insertMember(floatNullKey, DeclareParam , 2,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(identifierVar));
    insertMember(intNullKey, DeclareParam , 2,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(identifierVar));
    insertMember(stringKey, DeclareParam , 2,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(identifierVar));
    insertMember(floatKey, DeclareParam , 2,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(identifierVar));
    insertMember(intKey, DeclareParam , 2,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(identifierVar));
    insertMember(voidKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(voidKey));
    insertMember(stringNullKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(DataType));
    insertMember(floatNullKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(DataType));
    insertMember(intNullKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(DataType));
    insertMember(stringKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(DataType));
    insertMember(floatKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(DataType));
    insertMember(intKey, FuncReturnColonType , 1,
                 partOfRulesRightSide(DataType));
    insertMember(identifierFunc, FceCall , 4,
                 partOfRulesRightSide(identifierFunc),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(FirstFceParam),
                 partOfRulesRightSide(rightPar));
    insertMember(rightPar, FirstFceParam , 0);
    insertMember(identifierVar, FirstFceParam , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(floatLiteral, FirstFceParam , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(stringLiteral, FirstFceParam , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(integerLiteral, FirstFceParam , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(rightPar, CommaOrEpsParam , 0);
    insertMember(comma, CommaOrEpsParam , 3,
                 partOfRulesRightSide(comma),
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(identifierVar, Statement , 1,
                 partOfRulesRightSide(identifierVar));
    insertMember(floatLiteral, Statement , 1,
                 partOfRulesRightSide(floatLiteral));
    insertMember(stringLiteral, Statement , 1,
                 partOfRulesRightSide(stringLiteral));
    insertMember(integerLiteral, Statement , 1,
                 partOfRulesRightSide(integerLiteral));
    insertMember(identifierFunc, Exp , 2,
                 partOfRulesRightSide(FceCall),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp , 4,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp , 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp , 2,
                 partOfRulesRightSide(nullKey),
                 partOfRulesRightSide(Exp));
    insertMember(stringNullKey, DataType , 1,
                 partOfRulesRightSide(stringNullKey));
    insertMember(floatNullKey, DataType , 1,
                 partOfRulesRightSide(floatNullKey));
    insertMember(intNullKey, DataType , 1,
                 partOfRulesRightSide(intNullKey));
    insertMember(stringKey, DataType , 1,
                 partOfRulesRightSide(stringKey));
    insertMember(floatKey, DataType , 1,
                 partOfRulesRightSide(floatKey));
    insertMember(intKey, DataType , 1,
                 partOfRulesRightSide(intKey));
    insertMember(identifierVar, DeclareVariable , 2,
                 partOfRulesRightSide(identifierVar),
                 partOfRulesRightSide(DefVarAss));
    insertMember(semicolon, DefVarAss , 1,
                 partOfRulesRightSide(semicolon));
    insertMember(equals, DefVarAss , 3,
                 partOfRulesRightSide(equals),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(ifKey, Condition , 8,
                 partOfRulesRightSide(ifKey),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(rightPar),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight),
                 partOfRulesRightSide(ElseCond));
    insertMember(ending, ElseCond , 0);
    insertMember(semicolon, ElseCond , 0);
    insertMember(curlyBraceRight, ElseCond , 0);
    insertMember(functionKey, ElseCond , 0);
    insertMember(identifierFunc, ElseCond , 0);
    insertMember(leftPar, ElseCond , 0);
    insertMember(identifierVar, ElseCond , 0);
    insertMember(floatLiteral, ElseCond , 0);
    insertMember(stringLiteral, ElseCond , 0);
    insertMember(integerLiteral, ElseCond , 0);
    insertMember(nullKey, ElseCond , 0);
    insertMember(ifKey, ElseCond , 0);
    insertMember(elseKey, ElseCond , 4,
                 partOfRulesRightSide(elseKey),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight));
    insertMember(whileKey, ElseCond , 0);
    insertMember(returnKey, ElseCond , 0);
    insertMember(whileKey, While , 7,
                 partOfRulesRightSide(whileKey),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(rightPar),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight));
    insertMember(returnKey, Return , 3,
                 partOfRulesRightSide(returnKey),
                 partOfRulesRightSide(ReturnExp),
                 partOfRulesRightSide(semicolon));
    insertMember(semicolon, ReturnExp , 0);
    insertMember(identifierFunc, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, ReturnExp , 1,
                 partOfRulesRightSide(Exp));
    insertMember(semicolon, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(curlyBraceRight, FunctionBody , 0);
    insertMember(identifierFunc, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(leftPar, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(identifierVar, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(floatLiteral, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(stringLiteral, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(integerLiteral, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(nullKey, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(ifKey, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(whileKey, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(returnKey, FunctionBody , 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
}
