/**
 * @file stack.h
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief Generic stack implemented using void pointer
 * Implementation IFJ22 compiler
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "common.h"

#ifndef STACK_STACK_H
#define STACK_STACK_H

#define STACK_DEBUG 0



struct stackMem;
struct stackMem {
    void *data;
    struct stackMem *next;
};

typedef struct stackMem stackMem_t;

//stack
struct stack {
    int c;
    stackMem_t *top;
};

typedef struct stack genericStack;

genericStack *gStackInit();

int gStackPush(genericStack *s, void *nm);

void *gStackPop(genericStack *s);

bool gStackIsEmpty(genericStack *st);

stackMem_t *gStackBottom(genericStack *s, stackMem_t **pLast);

void *gStackPopBack(genericStack *s);


void gStackPrint(genericStack *s, void (*printMem)(void *));

void *gStackGetNth(genericStack *s, unsigned int numberForTop);

void*gStackTop(genericStack *s);

void gStackPushBefore(genericStack *s, unsigned elementNum, void *data);

void *getFromTop(genericStack *s, unsigned int numberForTop);


#endif//STACK_STACK_H
