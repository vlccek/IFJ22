/**
 * @file LLTable.c
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @brief Hlavičkový soubor tabulky
 * Implementace překladače imperativního jazyka IFJ21
 *
 */

#ifndef LUAINTERPRET_LLTABLE_H
#define LUAINTERPRET_LLTABLE_H

#include "lex.h"
#include "parser.h"
#include "common.h"

#define MAX_RULE_LEN 10
#define MAX_RULE_COUNT 10

#define AddToRightSide(terminal, nonterminal, countOfRule, member, ruleIndex) Table[nonTerminal][terminal]->rule[ruleIndex]->to[countOfRule] = member;
#define partOfRulesRightSide(name) createExsStackMember(name, getDataType(#name))

typedef struct rule{
    int id;
    bool nullable;
    nonTerminalType from;
    extendedStackMember *to[MAX_RULE_LEN];
}rule;

typedef struct tableMember{
    rule *rule[MAX_RULE_COUNT];
    nonTerminalType nonTerminal;
    lexType terminal;
}tableMember;

typedef tableMember* table[(int) nonTerminalCount][(int) lexTypeCount];

tableMember *getLLMember(nonTerminalType nonterm, lexType terminal);
void createLLTable();
tableMember *getLLMemberByRule(extendedStackMember *to[MAX_RULE_LEN], int *ruleIndex);

#endif //LUAINTERPRET_LLTABLE_H
