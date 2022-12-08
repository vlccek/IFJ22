/**
* @file LLTable.c
* @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
* @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
* @brief Table containing all rules
* Implementation IFJ22 compiler
*/

#ifndef LUAINTERPRET_LLTABLE_H
#define LUAINTERPRET_LLTABLE_H

#include "common.h"
#include "lex.h"
#include "semanticActionInfo.h"

#define MAX_RULE_LEN 15
#define MAX_RULES_IN_CELL 15

#define AddToRightSide(terminal, nonterminal, countOfRule, member, ruleIndex) \
    Table[nonTerminal][terminal]->rules[ruleIndex]->to[countOfRule] = member;
#define partOfRulesRightSide(name) createPSAStackMember(name, getDataType(#name))

typedef enum {
    // S - initial state
    ProgramBody,

    // Command
    Command,

    // Function definitions
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

    // Variable definitions
    DeclareVariable,
    DefVarAss,

    // Conditions
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
    endOfFile,// special token to detect end of stack
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
} rule;

typedef struct tableMember {
    rule *rules[MAX_RULES_IN_CELL];
    nonTerminalType nonTerminal;
    lexType terminal;
} tableMember;


typedef tableMember *table[(int) nonTerminalCount][(int) lexTypeCount];

tableMember *getLLMember(nonTerminalType nonterm, lexType terminal);

void createLLTable();

rule *findRuleByHandle(PSAStackMember *handleToFind[MAX_RULE_LEN]);

PSAStackMember *createPSAStackMember(int value, PSADataType type);

char *getStringPSAMember(PSAStackMember m);

void setSemanticActionAllRules(nonTerminalType nonTerminal,
                               void (*semanticAction)(semanticActionInfo));

void setSemanticAction(nonTerminalType nonTerminal,
                       lexType terminal,
                       void (*semanticAction)(semanticActionInfo));

void setSemanticActionRow(nonTerminalType nonTerminal,
                          void (*semanticAction)(semanticActionInfo),
                          size_t except,
                          ...);

#endif//LUAINTERPRET_LLTABLE_H
