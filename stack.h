//
// Created by jvlk on 20.9.22.
//
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
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
    typeOfStoredData data;
    struct stackMem *next;
};

typedef struct stackMem stackMem_t;

//stack
struct stack {
    int c;
    stackMem_t *top;
};

typedef struct stack stack_t;

stack_t *stack();

int push(stack_t *s, typeOfStoredData nm);

void freeStackEl(typeOfStoredData *fe);

typeOfStoredData pop(stack_t *s);

bool sIsEmpty(stack_t *st);

stackMem_t *stackBotton(stack_t *s, stackMem_t **pLast);

typeOfStoredData popBack(stack_t *s);

void printMember(typeOfStoredData *data);

void printStack(stack_t *s, void (*printMem)(typeOfStoredData *));


#endif //STACK_STACK_H
