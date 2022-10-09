//
// Created by tonda on 05/10/22.
//
#include "parser.h"
#include "LLtable.h"

void pushStackToStack(genericStack *original, genericStack *toEmpty) {
    while (stackTop(toEmpty) != NULL) {
        push(original, pop(toEmpty));
    }
}

///
/// \param memory
/// \param stackLength
/// \return Returns 0 if NULL inserted otherwise returns 1
int updateStackViewMember(ParserMemory *memory, int stackLength) {
    PSAStackMember *top = (PSAStackMember *) stackTop(memory->PSAStack);
    if (top == NULL) {
        memory->stackView[stackLength] = NULL;
        return 0;
    }
    memory->stackView[stackLength] = getStringPSAMember(*top);
    return 1;
}

void updateStackView(ParserMemory *memory) {
    make_var(tmpStack, genericStack*, sizeof(genericStack));

    for (int stackLength = 0; stackLength < MAX_STACK_VIEWABLE; ++stackLength) {
        if (updateStackViewMember(memory, stackLength) == 0)
            break;
        push(tmpStack, pop(memory->PSAStack));
    }
    pushStackToStack(memory->PSAStack, tmpStack);
    free(tmpStack);
}

void PSAStackInit(ParserMemory *mem) {
    mem->PSAStack = stackInit();
    push(mem->PSAStack, createPSAStackMember(0, endOfFile));
    push(mem->PSAStack, createPSAStackMember(ProgramBody, nonTerminal));
    updateStackView(mem);
}

void stackViewInit(ParserMemory *mem) {
    for (int i = 0; i < MAX_STACK_VIEWABLE; ++i) {
        mem->stackView[i] = NULL;
    }
}

ParserMemory *initializeMemory() {
    make_var(mem, ParserMemory*, sizeof(ParserMemory));
    PSAStackInit(mem);
    stackViewInit(mem);
    createLLTable();
    return mem;
}

void exitUnexpectedEnd(token_t token) {
    pErrSyntaxExit(token.type, token.rowNumber, token.rowPosNumber,
                   "Unexpected end of program.\n");
}

void exitNoRule(token_t token, nonTerminalType topOfTheStack) {
    pErrSyntaxExit(token.type, token.rowNumber, token.rowPosNumber,
                   "No rule found.\nOn top of stack is: %s.\n", getNonTerminalName(topOfTheStack));
}

void exitWrongToken(token_t actualToken, lexType expectedLexType) {
    pErrSyntaxExit(actualToken.type, actualToken.rowNumber, actualToken.rowPosNumber,
                   "Expected token is: '%s'\nInstead got token: ", getTerminalName(expectedLexType));
}


void pushReversed(genericStack *stack, PSAStackMember **rule) {
    make_var(tmpStack, genericStack*, sizeof(genericStack));
    while (*rule != NULL) {
        push(tmpStack, *rule);
        *rule++;
    }
    pushStackToStack(stack, tmpStack);
    free(tmpStack);
}


PSAStackMember *getTopStack(ParserMemory *memory) {
    PSAStackMember *m = (PSAStackMember *) stackTop(memory->PSAStack);
    updateStackView(memory);
    return m;
}


int parser() {
    ParserMemory *memory = initializeMemory();


    bool success = false;
    PSAStackMember *topOfStack;
    token_t lastToken;

    lastToken = nextToken(stdin);
    while (success == false) {
        topOfStack = getTopStack(memory);
        switch (topOfStack->type) {
            case endOfFile:
                if (lastToken.type == ending)
                    success = true;
                else
                    exitUnexpectedEnd(lastToken);
                break;
            case terminal:
                if (lastToken.type == topOfStack->data) {
                    pop(memory->PSAStack);
                    lastToken = nextToken(stdin);
                } else {
                    exitWrongToken(lastToken, (lexType) topOfStack->data);
                }
                break;
            case nonTerminal:;
                if (topOfStack->data == (int) Exp)
                    InternalError("\nExpression parser is not ready yet.");

                rule **newRule = (getLLMember((nonTerminalType) topOfStack->data, lastToken.type))->rules;
                if (newRule == NULL)
                    exitNoRule(lastToken, (nonTerminalType) topOfStack->data);
                if ((*newRule)->epsRule) {
                    pop(memory->PSAStack);
                } else {
                    pop(memory->PSAStack);
                    pushReversed(memory->PSAStack, (*newRule)->to);
                }
                break;
        }
    }

    return 0;
}

