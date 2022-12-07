/**
* @file stackIf.c
* @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
* @brief Wrapper pro stack.
 * Generates unique labels for jumps and memorizes state using stack
*/
#include "stackIf.h"
#define endlabelidIF "ENDE"
#define elseLbaelIdIF "ELSE"
#define endlabelidWHILE "ENDE"
#define elseLbaelIdWHILE "ELSE"
#define buffersize 100


genericStack *ifS_Init() {
    genericStack *gs = gStackInit();
    return gs;
}

void ifS_newIf(genericStack *stack) {
    static int c = 0;
    c++;

    ifsState *is = malloc(sizeof(ifsState));
    checkNullPointer(is);

    // add unikátnost
    char buff[buffersize] = {0};
    sprintf(buff, "%d", c);

    is->elselabel = dstrInitChar(elseLbaelIdIF);
    dstrAppend(is->elselabel, buff);
    is->endlabel = dstrInitChar(endlabelidIF);
    dstrAppend(is->endlabel, buff);
    is->isEndingGenerated = false;
    is->inElse = false;
    is->expectinElse = false;
    is->inIfbranch = false;
    is->type = if_type;

    gStackPush(stack, is);
}

void ifS_newWhile(genericStack *stack) {
    static int c = 0;
    c++;

    ifsState *is = malloc(sizeof(ifsState));
    checkNullPointer(is);

    // add unikátnost
    char buff[buffersize] = {0};
    sprintf(buff, "%d", c);

    is->elselabel = NULL;// else label is not use in while type
    is->endlabel = dstrInitChar(endlabelidIF);
    dstrAppend(is->endlabel, buff);

    is->inWhileBody = false;
    is->endOfWhile;
    is->type = while_type;

    gStackPush(stack, is);
}

void ifS_old(genericStack *stack) {
    gStackPop(stack);
}

dynStr_t *ifS_ending(genericStack *stack) {
    return ((ifsState *) gStackTop(stack))->endlabel;
}
dynStr_t *ifS_else(genericStack *stack) {
    return ((ifsState *) gStackTop(stack))->elselabel;
}

bool ifS_isEndingGenerated(genericStack *stack) {
    if (gStackIsEmpty(stack)) {
        return true;// znemaná že se negeneruje žádný if
    }
    return ((ifsState *) gStackTop(stack))->isEndingGenerated;
}

void ifS_isSetEndingGenerated(genericStack *stack, bool end) {
    ((ifsState *) gStackTop(stack))->isEndingGenerated = end;
}

bool ifS_expectingElse(genericStack *stack) {
    if (gStackIsEmpty(stack)) {
        return false;// znemaná že se negeneruje žádný if
    }
    return ((ifsState *) gStackTop(stack))->expectinElse;
}

void ifS_SetExpectingElse(genericStack *stack, bool end) {
    ((ifsState *) gStackTop(stack))->expectinElse = end;
}

bool ifS_inElse(genericStack *stack) {
    if (gStackIsEmpty(stack)) {
        return true;// znemaná že se negeneruje žádný if
    }
    return ((ifsState *) gStackTop(stack))->isEndingGenerated;
}

void ifS_SetsinElse(genericStack *stack, bool end) {
    ((ifsState *) gStackTop(stack))->isEndingGenerated = end;
}

bool ifS_inIfbranch(genericStack *stack) {
    if (gStackIsEmpty(stack)) {
        return false;// znemaná že se negeneruje žádný if
    }
    return ((ifsState *) gStackTop(stack))->inIfbranch;
}

void ifS_SetinIfbranch(genericStack *stack, bool end) {
    ((ifsState *) gStackTop(stack))->inIfbranch = end;
}

bool ifS_isEmpty(genericStack *stack) {
    return gStackIsEmpty(stack);
}

typeOfIfStackMem ifs_getType(genericStack *stack) {
    return ((ifsState *) gStackTop(stack))->type;
}
