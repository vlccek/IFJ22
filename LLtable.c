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
    checkNullPointer(Table[nonterm][terminal]);
    return Table[nonterm][terminal];
}

bool stackMemberCmp(PSAStackMember *memberA, PSAStackMember *memberB) {
    return (memberA->type == memberB->type && memberA->data == memberB->data);
}

int cmpRules(tableMember *tMember, PSAStackMember **rightSideOfRule) {
    for (int i = 0; i < MAX_RULES_IN_CELL; ++i) {
        for (int j = 0; j < MAX_RULE_LEN; ++j) {
            if (tMember->rule[i] == NULL) {
                break;
            }
            PSAStackMember *m = rightSideOfRule[j];
            if (tMember->rule[i]->to[j] == NULL && m == NULL) {
                if (j == 0) {
                    break;
                }
                return j;
            }
            if (tMember->rule[i]->to[j] == NULL || m == NULL) {
                break;
            }
            if (!stackMemberCmp(tMember->rule[i]->to[j], m)) {
                break;
            }
        }
    }
    return 0;
}

tableMember *getLLMemberByRule(PSAStackMember *to[MAX_RULE_LEN], int *ruleIndex) {
    tableMember *tableMember;
    nonTerminalType multiRule[] = {Exp};
    int multiRuleLen = sizeof(multiRule) / sizeof(nonTerminalType);

    for (int i = 0; i < multiRuleLen; ++i) {
        for (int token = 0; token < lexTypeCount; ++token) {
            if ((tableMember = Table[multiRule[i]][token]) == NULL) {
                continue;
            };
            if ((*ruleIndex = cmpRules(tableMember, to)) != 0)
                return tableMember;
        }
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
    r->nullable = memberCount == 0;
    Table[nonTerminal][terminal]->rule[ruleIndex] = r;

    if (r->nullable == false) {
        int i;
        for (i = 0; i < MAX_RULES_IN_CELL; ++i) {
            if (i < memberCount) {
                AddToRightSide(terminal, nonTerminal, i, va_arg(members, PSAStackMember * ), ruleIndex);
            } else {
                AddToRightSide(terminal, nonTerminal, i, NULL, ruleIndex);

            }
            // Table[nonTerminal][terminal]->rule[ruleIndex]->to[i] = ((extendedStackMember *) members);
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
        if (Table[nonTerminal][terminal]->rule[0] == NULL) // this should never happen
            InternalError("No rule in initialized member!");

        int i;
        for (i = 1; i < MAX_RULES_IN_CELL; ++i) {
            if (Table[nonTerminal][terminal]->rule[i] == NULL) {
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
                        Table[radekIndex][sloupecIndex]->rule.from);
                int i = 0;
                while (Table[radekIndex][sloupecIndex]->rule.to[i] != NULL && i < MAX_RULE_LEN) {
                    // vypíše typdat,value
                    fprintf(stderr, "\t\t%d;%d ", Table[radekIndex][sloupecIndex]->rule.to[i]->typeOfData,
                            Table[radekIndex][sloupecIndex]->rule.to[i]->value);
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

//endregion
//region Inserts to table
void createLLTable() {
    static int onlyOneCallAllowed = 0;
    if (onlyOneCallAllowed++ > 0)
        return;



}