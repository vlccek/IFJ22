//
// Created by jvlk on 20.9.22.
//
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "common.h"

#ifndef STACK_STACK_H
#define STACK_STACK_H



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

void* gStackTop(genericStack *s);

void gStackPushBefore(genericStack *s, unsigned elementNum, void *data);

#endif//STACK_STACK_H
