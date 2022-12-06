//
// Created by jvlk on 6.12.22.
//
#include "stackIf.h"
#define endlabelid "ENDE"
#define elseLbaelId "ELSE"
#define buffersize 100


genericStack *ifS_Init() {
    genericStack *gs = gStackInit();
    return gs;
}

void ifS_new(genericStack *stack) {
    static int c = 0;
    c++;

    ifsState *is = malloc(sizeof(ifsState));
    checkNullPointer(is);

    // add unikátnost
    char buff[buffersize] = {0};
    sprintf(buff, "%d", c);

    is->elselabel = dstrInitChar(elseLbaelId);
    dstrAppend(is->elselabel, buff);
    is->endlabel = dstrInitChar(endlabelid);
    dstrAppend(is->endlabel, buff);

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
