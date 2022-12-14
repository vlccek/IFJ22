/**
 * @file LLTable.c
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief Table containing all rules
 * Implementation IFJ22 compiler
 */
#include "LLtable.h"

table Table;

void setSemanticAction(nonTerminalType nonTerminal,
                       lexType terminal,
                       void (*semanticAction)(semanticActionInfo)) {
    if (Table[nonTerminal][terminal]->rules[0]->semanticAction != NULL)
        InternalError("Prepisujes si semantickou akci");
    Table[nonTerminal][terminal]->rules[0]->semanticAction = semanticAction;
}

void setSemanticActionRow(nonTerminalType nonTerminal,
                          void (*semanticAction)(semanticActionInfo),
                          size_t except, ...) {
    for (int i = 0; i < lexTypeCount; ++i) {
        if (Table[nonTerminal][i]) {
            Table[nonTerminal][i]->rules[0]->semanticAction = semanticAction;
        }
    }
    va_list list;
    va_start(list, except);

    for (int x = 0; x < except; x++) {
        lexType toDelete = va_arg(list, lexType);
        if (Table[nonTerminal][toDelete]) {
            Table[nonTerminal][toDelete]->rules[0]->semanticAction = NULL;
        }
    }
    va_end(list);
}

void setSemanticActionAllRules(nonTerminalType nonTerminal,
                               void (*semanticAction)(semanticActionInfo)) {
    // func is called ony to insert Exp actions
    // and first is statement, so first should be skipped
    int firstRule = 1;
    // the great pyramid living its happy life
    for (int lexType = 0; lexType < lexTypeCount; ++lexType) {
        if (Table[nonTerminal][lexType]) {
            for (int rule = firstRule; rule < MAX_RULES_IN_CELL; ++rule) {
                if (Table[nonTerminal][lexType]->rules[rule]) {
                    Table[nonTerminal][lexType]->rules[rule]->semanticAction = semanticAction;
                }
            }
        }
    }
}

tableMember *getLLMember(nonTerminalType nonterm, lexType terminal) {
    if (nonterm >= nonTerminalCount || terminal >= lexTypeCount)
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
    nonTerminalType tableRows[] = {Exp, Statement};// looking only in this rows
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
    make_var(member, PSAStackMember *, sizeof(PSAStackMember));
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


void InserRules(int terminal,
                int nonTerminal,
                int memberCount,
                va_list members,
                int ruleIndex) {
    rule *r = (rule *) malloc(sizeof(rule));
    checkNullPointer(r);
    static int counter = 0;
    r->id = counter++;
    r->from = nonTerminal;
    r->epsRule = memberCount == 0;
    r->semanticAction = NULL;
    Table[nonTerminal][terminal]->rules[ruleIndex] = r;

    if (r->epsRule == false) {
        int i;
        for (i = 0; i < MAX_RULES_IN_CELL; ++i) {
            if (i < memberCount) {
                AddToRightSide(terminal, nonTerminal, i,
                               va_arg(members, PSAStackMember *), ruleIndex);
            } else {
                AddToRightSide(terminal, nonTerminal, i, NULL, ruleIndex);
            }
            // Table[nonTerminal][terminal]->rules[ruleIndex]->to[i] = ((extendedStackMember *) members);
        }
        if (i < MAX_RULE_LEN) {
            // last is null only if not entirely full
            AddToRightSide(terminal, nonTerminal, i, NULL, ruleIndex);
        }
        va_end(members);
    }
}

void insertMember(lexType terminal, nonTerminalType nonTerminal, int memberCount, ...) {
    va_list members;
    va_start(members, memberCount);

    if (memberCount > MAX_RULE_LEN) {
        InternalError("Trying to insert more rules than possible. (Low MAX_RULE_LEN).");
    }

    if (Table[nonTerminal][terminal] != NULL) {
        if (Table[nonTerminal][terminal]->rules[0] == NULL)// this should never happen
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

PSADataType getDataType(char *name) {
    for (int i = 0; i < nonTerminalCount; ++i) {
        if (!strcmp(name, getNonTerminalName(i)))
            return nonTerminal;
    }
    return terminal;
}

char *getStringPSAMember(PSAStackMember m) {
    if (m.type == terminal)
        return getTerminalName(m.data);
    if (m.type == nonTerminal)
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
    insertMember(semicolon, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(functionKey, ProgramBody, 2,
                 partOfRulesRightSide(FceDefine),
                 partOfRulesRightSide(ProgramBody));
    insertMember(identifierFunc, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(leftPar, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(identifierVar, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(floatLiteral, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(stringLiteral, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(integerLiteral, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(nullKey, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(ifKey, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(whileKey, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(returnKey, ProgramBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(ProgramBody));
    insertMember(semicolon, Command, 1,
                 partOfRulesRightSide(semicolon));
    insertMember(identifierFunc, Command, 1,
                 partOfRulesRightSide(FceCall));
    insertMember(leftPar, Command, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(identifierVar, Command, 1,
                 partOfRulesRightSide(DeclareVariable));
    insertMember(identifierVar, Command, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(floatLiteral, Command, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(stringLiteral, Command, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(integerLiteral, Command, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(nullKey, Command, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(ifKey, Command, 1,
                 partOfRulesRightSide(Condition));
    insertMember(whileKey, Command, 1,
                 partOfRulesRightSide(While));
    insertMember(returnKey, Command, 1,
                 partOfRulesRightSide(Return));
    insertMember(functionKey, FceDefine, 5,
                 partOfRulesRightSide(functionKey),
                 partOfRulesRightSide(FceHeader),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight));
    insertMember(identifierFunc, FceHeader, 6,
                 partOfRulesRightSide(identifierFunc),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(FunctionDeclareParams),
                 partOfRulesRightSide(rightPar),
                 partOfRulesRightSide(colon),
                 partOfRulesRightSide(FuncReturnColonType));
    insertMember(rightPar, FunctionDeclareParams, 0);
    insertMember(stringNullKey, FunctionDeclareParams, 3,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(floatNullKey, FunctionDeclareParams, 3,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(intNullKey, FunctionDeclareParams, 3,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(stringKey, FunctionDeclareParams, 3,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(floatKey, FunctionDeclareParams, 3,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(intKey, FunctionDeclareParams, 3,
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(rightPar, CommaOrEpsParams, 0);
    insertMember(comma, CommaOrEpsParams, 4,
                 partOfRulesRightSide(comma),
                 partOfRulesRightSide(DataType),
                 partOfRulesRightSide(DeclareParam),
                 partOfRulesRightSide(CommaOrEpsParams));
    insertMember(identifierVar, DeclareParam, 1,
                 partOfRulesRightSide(identifierVar));
    insertMember(voidKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(voidKey));
    insertMember(stringNullKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(DataType));
    insertMember(floatNullKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(DataType));
    insertMember(intNullKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(DataType));
    insertMember(stringKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(DataType));
    insertMember(floatKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(DataType));
    insertMember(intKey, FuncReturnColonType, 1,
                 partOfRulesRightSide(DataType));
    insertMember(identifierFunc, FceCall, 4,
                 partOfRulesRightSide(identifierFunc),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(FirstFceParam),
                 partOfRulesRightSide(rightPar));
    insertMember(rightPar, FirstFceParam, 0);
    insertMember(identifierVar, FirstFceParam, 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(floatLiteral, FirstFceParam, 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(stringLiteral, FirstFceParam, 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(integerLiteral, FirstFceParam, 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(nullKey, FirstFceParam, 2,
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(rightPar, CommaOrEpsParam, 0);
    insertMember(comma, CommaOrEpsParam, 3,
                 partOfRulesRightSide(comma),
                 partOfRulesRightSide(Statement),
                 partOfRulesRightSide(CommaOrEpsParam));
    insertMember(identifierVar, Statement, 1,
                 partOfRulesRightSide(identifierVar));
    insertMember(floatLiteral, Statement, 1,
                 partOfRulesRightSide(floatLiteral));
    insertMember(stringLiteral, Statement, 1,
                 partOfRulesRightSide(stringLiteral));
    insertMember(integerLiteral, Statement, 1,
                 partOfRulesRightSide(integerLiteral));
    insertMember(nullKey, Statement, 1,
                 partOfRulesRightSide(nullKey));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(plusOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(divisionOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(multiplicationOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(concatenationOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(eqOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(notEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(leftPar, Exp, 3,
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(rightPar));
    insertMember(identifierVar, Exp, 1,
                 partOfRulesRightSide(Statement));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(plusOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(divisionOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(multiplicationOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(concatenationOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(eqOp),
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(notEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 1,
                 partOfRulesRightSide(Statement));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(plusOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(divisionOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(multiplicationOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(concatenationOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(eqOp),
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(notEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 1,
                 partOfRulesRightSide(Statement));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(plusOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(divisionOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(multiplicationOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(concatenationOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(eqOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(notEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 1,
                 partOfRulesRightSide(Statement));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(plusOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(divisionOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(multiplicationOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(concatenationOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(eqOp),
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(notEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 1,
                 partOfRulesRightSide(Statement));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(minusOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(plusOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(divisionOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(multiplicationOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(concatenationOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(lesserEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterThanOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(greaterEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(eqOp),
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, Exp, 3,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(notEqOp),
                 partOfRulesRightSide(Exp));
    insertMember(stringNullKey, DataType, 1,
                 partOfRulesRightSide(stringNullKey));
    insertMember(floatNullKey, DataType, 1,
                 partOfRulesRightSide(floatNullKey));
    insertMember(intNullKey, DataType, 1,
                 partOfRulesRightSide(intNullKey));
    insertMember(stringKey, DataType, 1,
                 partOfRulesRightSide(stringKey));
    insertMember(floatKey, DataType, 1,
                 partOfRulesRightSide(floatKey));
    insertMember(intKey, DataType, 1,
                 partOfRulesRightSide(intKey));
    insertMember(identifierVar, DeclareVariable, 3,
                 partOfRulesRightSide(identifierVar),
                 partOfRulesRightSide(equals),
                 partOfRulesRightSide(DefVarAss));
    insertMember(identifierFunc, DefVarAss, 2,
                 partOfRulesRightSide(FceCall),
                 partOfRulesRightSide(semicolon));
    insertMember(leftPar, DefVarAss, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(identifierVar, DefVarAss, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(floatLiteral, DefVarAss, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(stringLiteral, DefVarAss, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(integerLiteral, DefVarAss, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(nullKey, DefVarAss, 2,
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(semicolon));
    insertMember(ifKey, Condition, 8,
                 partOfRulesRightSide(ifKey),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(rightPar),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight),
                 partOfRulesRightSide(ElseCond));
    insertMember(ending, ElseCond, 0);
    insertMember(semicolon, ElseCond, 0);
    insertMember(functionKey, ElseCond, 0);
    insertMember(curlyBraceRight, ElseCond, 0);
    insertMember(identifierFunc, ElseCond, 0);
    insertMember(leftPar, ElseCond, 0);
    insertMember(identifierVar, ElseCond, 0);
    insertMember(floatLiteral, ElseCond, 0);
    insertMember(stringLiteral, ElseCond, 0);
    insertMember(integerLiteral, ElseCond, 0);
    insertMember(nullKey, ElseCond, 0);
    insertMember(ifKey, ElseCond, 0);
    insertMember(elseKey, ElseCond, 4,
                 partOfRulesRightSide(elseKey),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight));
    insertMember(whileKey, ElseCond, 0);
    insertMember(returnKey, ElseCond, 0);
    insertMember(whileKey, While, 7,
                 partOfRulesRightSide(whileKey),
                 partOfRulesRightSide(leftPar),
                 partOfRulesRightSide(Exp),
                 partOfRulesRightSide(rightPar),
                 partOfRulesRightSide(curlyBraceLeft),
                 partOfRulesRightSide(FunctionBody),
                 partOfRulesRightSide(curlyBraceRight));
    insertMember(returnKey, Return, 3,
                 partOfRulesRightSide(returnKey),
                 partOfRulesRightSide(ReturnExp),
                 partOfRulesRightSide(semicolon));
    insertMember(semicolon, ReturnExp, 0);
    insertMember(leftPar, ReturnExp, 1,
                 partOfRulesRightSide(Exp));
    insertMember(identifierVar, ReturnExp, 1,
                 partOfRulesRightSide(Exp));
    insertMember(floatLiteral, ReturnExp, 1,
                 partOfRulesRightSide(Exp));
    insertMember(stringLiteral, ReturnExp, 1,
                 partOfRulesRightSide(Exp));
    insertMember(integerLiteral, ReturnExp, 1,
                 partOfRulesRightSide(Exp));
    insertMember(nullKey, ReturnExp, 1,
                 partOfRulesRightSide(Exp));
    insertMember(semicolon, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(curlyBraceRight, FunctionBody, 0);
    insertMember(identifierFunc, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(leftPar, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(identifierVar, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(floatLiteral, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(stringLiteral, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(integerLiteral, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(nullKey, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(ifKey, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(whileKey, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
    insertMember(returnKey, FunctionBody, 2,
                 partOfRulesRightSide(Command),
                 partOfRulesRightSide(FunctionBody));
}
