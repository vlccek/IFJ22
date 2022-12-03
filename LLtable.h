/**
 * @file LLTable.c
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief Hlavičkový soubor tabulky
 * Implementace překladače jazyka IFJ22
 */

#ifndef LUAINTERPRET_LLTABLE_H
#define LUAINTERPRET_LLTABLE_H

#include "lex.h"
#include "common.h"
#include "semanticActionInfo.h"

#define MAX_RULE_LEN 10
#define MAX_RULES_IN_CELL 10

#define AddToRightSide(terminal, nonterminal, countOfRule, member, ruleIndex) Table[nonTerminal][terminal]->rules[ruleIndex]->to[countOfRule] = member;
#define partOfRulesRightSide(name) createPSAStackMember(name, getDataType(#name))

typedef enum {
    // S - init stav
    ProgramBody,

    // Command
    Command,

    // Definice funkcí
    FceDefine,
    FceHeader,
    FunctionDeclareParams,
    CommaOrEpsParams,
    DeclareParam,

    // FuncReturnType
    FuncReturnColonType,

    // Function call
    FceCall,
    FirstFceParam,
    CommaOrEpsParam,

    // Exp
    Exp,
    Statement,

    // Data types DataType
    DataType,

    // Definice proměné
    DeclareVariable,
    DefVarAss,

    // Podmínky
    Condition,
    ElseCond,

    // While
    While,

    // Return
    Return,
    ReturnExp,

    // Vnítřek funkce
    FunctionBody,
    nonTerminalCount
} nonTerminalType;

// Ll table
typedef enum PSADataType {
    endOfFile, // special token to detect end of stack
    terminal,
    nonTerminal
} PSADataType;

typedef struct PSAStackMember {
    int data;
    PSADataType type;
} PSAStackMember;

typedef struct rule {
    int id;
    bool epsRule;

    void (*semanticAction)(semanticActionInfo);

    nonTerminalType from;
    PSAStackMember *to[MAX_RULE_LEN];
}rule;

typedef struct tableMember{
    rule *rules[MAX_RULES_IN_CELL];
    nonTerminalType nonTerminal;
    lexType terminal;
}tableMember;


typedef tableMember *table[(int) nonTerminalCount][(int) lexTypeCount];

tableMember *getLLMember(nonTerminalType nonterm, lexType terminal);

void createLLTable();

rule *findRuleByHandle(PSAStackMember *handleToFind[MAX_RULE_LEN]);

PSAStackMember *createPSAStackMember(int value, PSADataType type);

char *getStringPSAMember(PSAStackMember m);

void setSemanticAction(nonTerminalType nonTerminal, lexType terminal, void (*semanticAction)(semanticActionInfo));

#endif //LUAINTERPRET_LLTABLE_H
