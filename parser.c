//
// Created by tonda on 05/10/22.
//
#include "parser.h"
#include "LLtable.h"


void PSAStackInit(ParserMemory* mem) {
    mem->PSAStack = gStackInit();
    gStackPush(mem->PSAStack, createPSAStackMember(0, endOfFile));
    gStackPush(mem->PSAStack, createPSAStackMember(ProgramBody, nonTerminal));
}

ParserMemory* initializeMemory() {
    make_var(mem, ParserMemory*, sizeof (ParserMemory));
    PSAStackInit(mem);
    return mem;
}

void unxpectedEnd(token_t token) {
    pErrSyntax(token.type, token.rowNumber, token.rowPosNumber,
               "Unxpected end of program.\n");
}

void pushStackToStack(genericStack* original, genericStack* toEmpty){
    while(gStackTop(toEmpty) != NULL){
        gStackPush(original, gStackPop(toEmpty));
    }
}

void pushReversed(genericStack* stack, PSAStackMember** rule){
    make_var(tmpStack, genericStack*, sizeof (genericStack));
    while (*rule != NULL){
        gStackPush(tmpStack, *rule);
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
        topOfStack = (PSAStackMember *) gStackTop(memory->PSAStack);
        switch (topOfStack->type) {
            case endOfFile:
                if(lastToken.type == ending)
                    success = true;
                else
                    unxpectedEnd(lastToken);
                break;
            case terminal:
                if(lastToken.type == topOfStack->data){
                    gStackPop(memory->PSAStack);
                    lastToken = nextToken(stdin);
                }
                break;
            case nonTerminal:;
                PSAStackMember** newRule = (PSAStackMember **) (getLLMember((nonTerminalType) topOfStack->data, lastToken.type))->rule;
                pushReversed(memory->PSAStack, newRule);
                break;
        }
    }
    return 0;
}

