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

/*
 * Returns negative number when comparing failed
 * Returns rule index otherwise
 * */
int findHandleInTableMember(tableMember *tMember, PSAStackMember **rightSideOfRule) {
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
                return i;
            }
            if (tMember->rules[i]->to[j] == NULL || m == NULL) {
                break;
            }
            if (!stackMemberCmp(tMember->rules[i]->to[j], m)) {
                break;
            }
        }
    }
    return -1;
}

rule *findInTable(PSAStackMember *handleToFind[10], nonTerminalType tableRow) {
    tableMember *tableMember;
    int ruleIndex;

    for (lexType token = 0; token < lexTypeCount; ++token) {
        if ((tableMember = Table[tableRow][token]) == NULL) {
            continue;
        };
        if ((ruleIndex = findHandleInTableMember(tableMember, handleToFind)) >= 0)
            return tableMember->rules[ruleIndex];
    }
    return NULL;
}

rule *findRuleByHandle(PSAStackMember *handleToFind[MAX_RULE_LEN]) {
    nonTerminalType tableRows[] = {Exp, Statement}; // looking only in this rows
    int multiRuleLen = sizeof(tableRows) / sizeof(nonTerminalType);
    rule *found;
    for (int i = 0; i < multiRuleLen; ++i) {
        if ((found = findInTable(handleToFind, tableRows[i])) != NULL)
            return found;
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

    insertMember(ending, ProgramBody, 0);
    insertMember(semicolon, ProgramBody, 2, Command, ProgramBody);
    insertMember(functionKey, ProgramBody, 2, FceDefine, ProgramBody);
    insertMember(identifierFunc, ProgramBody, 2, Command, ProgramBody);
    insertMember(leftPar, ProgramBody, 2, Command, ProgramBody);
    insertMember(identifierVar, ProgramBody, 2, Command, ProgramBody);
    insertMember(floatLiteral, ProgramBody, 2, Command, ProgramBody);
    insertMember(stringLiteral, ProgramBody, 2, Command, ProgramBody);
    insertMember(integerLiteral, ProgramBody, 2, Command, ProgramBody);
    insertMember(nullKey, ProgramBody, 2, Command, ProgramBody);
    insertMember(ifKey, ProgramBody, 2, Command, ProgramBody);
    insertMember(whileKey, ProgramBody, 2, Command, ProgramBody);
    insertMember(returnKey, ProgramBody, 2, Command, ProgramBody);
    insertMember(semicolon, Command, 1, semicolon);
    insertMember(identifierFunc, Command, 1, FceCall);
    insertMember(leftPar, Command, 1, Exp);
    insertMember(identifierVar, Command, 2, DeclareVariable, Command);
    insertMember(identifierVar, Command, 1, Exp);
    insertMember(floatLiteral, Command, 1, Exp);
    insertMember(stringLiteral, Command, 1, Exp);
    insertMember(integerLiteral, Command, 1, Exp);
    insertMember(nullKey, Command, 1, Exp);
    insertMember(ifKey, Command, 1, Condition);
    insertMember(whileKey, Command, 1, While);
    insertMember(returnKey, Command, 1, Return);
    insertMember(functionKey, FceDefine, 4, FceHeader, curlyBraceLeft, FunctionBody, curlyBraceRight);
    insertMember(functionKey, FceHeader, 7, functionKey, identifierFunc, leftPar, FunctionDeclareParams, rightPar, colon, FuncReturnColonType);
    insertMember(rightPar, FunctionDeclareParams, 0);
    insertMember(stringNullKey, FunctionDeclareParams, 2, DeclareParam, CommaOrEpsParams);
    insertMember(floatNullKey, FunctionDeclareParams, 2, DeclareParam, CommaOrEpsParams);
    insertMember(intNullKey, FunctionDeclareParams, 2, DeclareParam, CommaOrEpsParams);
    insertMember(stringKey, FunctionDeclareParams, 2, DeclareParam, CommaOrEpsParams);
    insertMember(floatKey, FunctionDeclareParams, 2, DeclareParam, CommaOrEpsParams);
    insertMember(intKey, FunctionDeclareParams, 2, DeclareParam, CommaOrEpsParams);
    insertMember(rightPar, CommaOrEpsParams, 0);
    insertMember(comma, CommaOrEpsParams, 3, comma, DeclareParam, CommaOrEpsParams);
    insertMember(stringNullKey, DeclareParam, 2, DataType, identifierVar);
    insertMember(floatNullKey, DeclareParam, 2, DataType, identifierVar);
    insertMember(intNullKey, DeclareParam, 2, DataType, identifierVar);
    insertMember(stringKey, DeclareParam, 2, DataType, identifierVar);
    insertMember(floatKey, DeclareParam, 2, DataType, identifierVar);
    insertMember(intKey, DeclareParam, 2, DataType, identifierVar);
    insertMember(voidKey, FuncReturnColonType, 1, voidKey);
    insertMember(stringNullKey, FuncReturnColonType, 1, DataType);
    insertMember(floatNullKey, FuncReturnColonType, 1, DataType);
    insertMember(intNullKey, FuncReturnColonType, 1, DataType);
    insertMember(stringKey, FuncReturnColonType, 1, DataType);
    insertMember(floatKey, FuncReturnColonType, 1, DataType);
    insertMember(intKey, FuncReturnColonType, 1, DataType);
    insertMember(identifierFunc, FceCall, 4, identifierFunc, leftPar, FirstFceParam, rightPar);
    insertMember(rightPar, FirstFceParam, 0);
    insertMember(identifierVar, FirstFceParam, 2, Statement, CommaOrEpsParam);
    insertMember(floatLiteral, FirstFceParam, 2, Statement, CommaOrEpsParam);
    insertMember(stringLiteral, FirstFceParam, 2, Statement, CommaOrEpsParam);
    insertMember(integerLiteral, FirstFceParam, 2, Statement, CommaOrEpsParam);
    insertMember(rightPar, CommaOrEpsParam, 0);
    insertMember(comma, CommaOrEpsParam, 3, comma, Statement, CommaOrEpsParam);
    insertMember(identifierVar, Statement, 1, identifierVar);
    insertMember(floatLiteral, Statement, 1, floatLiteral);
    insertMember(stringLiteral, Statement, 1, stringLiteral);
    insertMember(integerLiteral, Statement, 1, integerLiteral);
    insertMember(leftPar, Exp, 3, Exp, minusOp, Exp);
    insertMember(leftPar, Exp, 3, Exp, plusOp, Exp);
    insertMember(leftPar, Exp, 3, Exp, divisionOp, Exp);
    insertMember(leftPar, Exp, 3, Exp, multiplicationOp, Exp);
    insertMember(leftPar, Exp, 3, Exp, concatenationOp, Exp);
    insertMember(leftPar, Exp, 3, leftPar, Exp, rightPar);
    insertMember(identifierVar, Exp, 2, Statement, Exp);
    insertMember(identifierVar, Exp, 3, Exp, minusOp, Exp);
    insertMember(identifierVar, Exp, 3, Exp, plusOp, Exp);
    insertMember(identifierVar, Exp, 3, Exp, divisionOp, Exp);
    insertMember(identifierVar, Exp, 3, Exp, multiplicationOp, Exp);
    insertMember(identifierVar, Exp, 2, Exp, concatenationOp);
    insertMember(floatLiteral, Exp, 2, Statement, Exp);
    insertMember(floatLiteral, Exp, 3, Exp, minusOp, Exp);
    insertMember(floatLiteral, Exp, 3, Exp, plusOp, Exp);
    insertMember(floatLiteral, Exp, 3, Exp, divisionOp, Exp);
    insertMember(floatLiteral, Exp, 3, Exp, multiplicationOp, Exp);
    insertMember(floatLiteral, Exp, 2, Exp, concatenationOp);
    insertMember(stringLiteral, Exp, 2, Statement, Exp);
    insertMember(stringLiteral, Exp, 3, Exp, minusOp, Exp);
    insertMember(stringLiteral, Exp, 3, Exp, plusOp, Exp);
    insertMember(stringLiteral, Exp, 3, Exp, divisionOp, Exp);
    insertMember(stringLiteral, Exp, 3, Exp, multiplicationOp, Exp);
    insertMember(stringLiteral, Exp, 2, Exp, concatenationOp);
    insertMember(integerLiteral, Exp, 2, Statement, Exp);
    insertMember(integerLiteral, Exp, 3, Exp, minusOp, Exp);
    insertMember(integerLiteral, Exp, 3, Exp, plusOp, Exp);
    insertMember(integerLiteral, Exp, 3, Exp, divisionOp, Exp);
    insertMember(integerLiteral, Exp, 3, Exp, multiplicationOp, Exp);
    insertMember(integerLiteral, Exp, 2, Exp, concatenationOp);
    insertMember(nullKey, Exp, 2, nullKey, Exp);
    insertMember(nullKey, Exp, 3, Exp, minusOp, Exp);
    insertMember(nullKey, Exp, 3, Exp, plusOp, Exp);
    insertMember(nullKey, Exp, 3, Exp, divisionOp, Exp);
    insertMember(nullKey, Exp, 3, Exp, multiplicationOp, Exp);
    insertMember(nullKey, Exp, 2, Exp, concatenationOp);
    insertMember(stringNullKey, DataType, 1, stringNullKey);
    insertMember(floatNullKey, DataType, 1, floatNullKey);
    insertMember(intNullKey, DataType, 1, intNullKey);
    insertMember(stringKey, DataType, 1, stringKey);
    insertMember(floatKey, DataType, 1, floatKey);
    insertMember(intKey, DataType, 1, intKey);
    insertMember(identifierVar, DeclareVariable, 3, identifierVar, equals, DefVarAss);
    insertMember(semicolon, DefVarAss, 1, semicolon);
    insertMember(identifierFunc, DefVarAss, 2, FceCall, semicolon);
    insertMember(leftPar, DefVarAss, 1, Exp);
    insertMember(identifierVar, DefVarAss, 1, Exp);
    insertMember(floatLiteral, DefVarAss, 1, Exp);
    insertMember(stringLiteral, DefVarAss, 1, Exp);
    insertMember(integerLiteral, DefVarAss, 1, Exp);
    insertMember(nullKey, DefVarAss, 1, Exp);
    insertMember(ifKey, Condition, 8, ifKey, leftPar, Exp, rightPar, curlyBraceLeft, FunctionBody, curlyBraceRight, ElseCond);
    insertMember(ending, ElseCond, 0);
    insertMember(semicolon, ElseCond, 0);
    insertMember(curlyBraceRight, ElseCond, 0);
    insertMember(functionKey, ElseCond, 0);
    insertMember(identifierFunc, ElseCond, 0);
    insertMember(leftPar, ElseCond, 0);
    insertMember(identifierVar, ElseCond, 0);
    insertMember(floatLiteral, ElseCond, 0);
    insertMember(stringLiteral, ElseCond, 0);
    insertMember(integerLiteral, ElseCond, 0);
    insertMember(nullKey, ElseCond, 0);
    insertMember(ifKey, ElseCond, 0);
    insertMember(elseKey, ElseCond, 4, elseKey, curlyBraceLeft, FunctionBody, curlyBraceRight);
    insertMember(whileKey, ElseCond, 0);
    insertMember(returnKey, ElseCond, 0);
    insertMember(whileKey, While, 7, whileKey, leftPar, Exp, rightPar, curlyBraceLeft, FunctionBody, curlyBraceRight);
    insertMember(returnKey, Return, 3, returnKey, ReturnExp, semicolon);
    insertMember(semicolon, ReturnExp, 0);
    insertMember(leftPar, ReturnExp, 0);
    insertMember(identifierVar, ReturnExp, 0);
    insertMember(floatLiteral, ReturnExp, 0);
    insertMember(stringLiteral, ReturnExp, 0);
    insertMember(integerLiteral, ReturnExp, 0);
    insertMember(nullKey, ReturnExp, 0);
    insertMember(semicolon, FunctionBody, 2, Command, FunctionBody);
    insertMember(curlyBraceRight, FunctionBody, 0);
    insertMember(identifierFunc, FunctionBody, 2, Command, FunctionBody);
    insertMember(leftPar, FunctionBody, 2, Command, FunctionBody);
    insertMember(identifierVar, FunctionBody, 2, Command, FunctionBody);
    insertMember(floatLiteral, FunctionBody, 2, Command, FunctionBody);
    insertMember(stringLiteral, FunctionBody, 2, Command, FunctionBody);
    insertMember(integerLiteral, FunctionBody, 2, Command, FunctionBody);
    insertMember(nullKey, FunctionBody, 2, Command, FunctionBody);
    insertMember(ifKey, FunctionBody, 2, Command, FunctionBody);
    insertMember(whileKey, FunctionBody, 2, Command, FunctionBody);
    insertMember(returnKey, FunctionBody, 2, Command, FunctionBody);
}
