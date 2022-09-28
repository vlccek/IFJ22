//
// Created by jvlk on 20.9.22.
//
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "helper.h"

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

typedef struct stack stack_type;

stack_type *stackInit();

int push(stack_type *s, void *nm);

void *pop(stack_type *s);

bool sIsEmpty(stack_type *st);

stackMem_t *stackBottom(stack_type *s, stackMem_t **pLast);

void *popBack(stack_type *s);

void printMember(void *data);

void printStack(stack_type *s, void (*printMem)(void *));

#endif //STACK_STACK_H
