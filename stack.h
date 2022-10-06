//
// Created by jvlk on 20.9.22.
//
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "common.h"

#ifndef STACK_STACK_H
#define STACK_STACK_H

struct ints2 {
    int a;
    int b;
};

typedef struct ints2 typeOfStoredData;

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

genericStack *stackInit();

int push(genericStack *s, void *nm);

void *pop(genericStack *s);

bool sIsEmpty(genericStack *st);

stackMem_t *stackBottom(genericStack *s, stackMem_t **pLast);

void *popBack(genericStack *s);

void printMember(void *data);

void printStack(genericStack *s, void (*printMem)(void *));

void* stackTop(genericStack *s);

#endif //STACK_STACK_H
