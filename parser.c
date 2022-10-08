//
// Created by tonda on 05/10/22.
//
#include "parser.h"
#include "LLtable.h"


void PSAStackInit(ParserMemory* mem) {
    mem->PSAStack = stackInit();
    push(mem->PSAStack, createPSAStackMember(0, endOfFile));
    push(mem->PSAStack, createPSAStackMember(ProgramBody, nonTerminal));
}

ParserMemory* initializeMemory() {
    make_var(mem, ParserMemory*, sizeof (ParserMemory));
    PSAStackInit(mem);
    createLLTable();
    return mem;
}

void exitUnxpectedEnd(token_t token) {
    pErrSyntax(token.type, token.rowNumber, token.rowPosNumber,
               "Unxpected end of program.\n");
}

void exitNoRule(token_t token, nonTerminalType topOfTheStack) {
    pErrSyntax(token.type, token.rowNumber, token.rowPosNumber,
               "No rule found.\nOn top of stack is: %s.\n", getNonTerminalName(topOfTheStack));
}

void pushStackToStack(genericStack *original, genericStack *toEmpty) {
    while (stackTop(toEmpty) != NULL) {
        push(original, pop(toEmpty));
    }
}

void pushReversed(genericStack *stack, PSAStackMember **rule) {
    make_var(tmpStack, genericStack*, sizeof(genericStack));
    while (*rule != NULL) {
        push(tmpStack, *rule);
        *rule++;
    }
    pushStackToStack(stack, tmpStack);
}

int parser() {
    ParserMemory* memory =  initializeMemory();

    bool success = false;
    PSAStackMember *topOfStack;
    token_t lastToken;

    lastToken = nextToken(stdin);
    while (success == false) {
        topOfStack = (PSAStackMember *) stackTop(memory->PSAStack);
        switch (topOfStack->type) {
            case endOfFile:
                if (lastToken.type == ending)
                    success = true;
                else
                    exitUnxpectedEnd(lastToken);
                break;
            case terminal:
                if (lastToken.type == topOfStack->data) {
                    pop(memory->PSAStack);
                    lastToken = nextToken(stdin);
                }
                break;
            case nonTerminal:;
                rule **newRule = (getLLMember((nonTerminalType) topOfStack->data, lastToken.type))->rule;
                if (newRule == NULL)
                    exitNoRule(lastToken, (nonTerminalType) topOfStack->data);
                if ((*newRule)->epsRule) {
                    pop(memory->PSAStack);
                } else {
                    pushReversed(memory->PSAStack, (*newRule)->to);
                }
                break;
        }
    }

    return 0;
}

