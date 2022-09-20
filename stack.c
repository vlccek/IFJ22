//
// Created by jvlk on 16.9.22.
//
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
#include "helper.h"

#define stack_member_null -2

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

stack_t *stack() {
    make_var(stack, stack_t*, sizeof(stack_t))
    stack->c = 0;
    stack->top = NULL;
    return stack;
}

//

int push(stack_t *s, typeOfStoredData nm) {
    if (!s) {
        //Stack is null
        return null_dereference;
    }

    stackMem_t *oldtop = s->top; // saves old top

    make_var(newtop, stackMem_t*, sizeof(stackMem_t))
    s->top = newtop;

    s->top->data = nm; // set data of new top
    s->top->next = oldtop; // point from old top to new top
    ///////////////////////// could be null!!!!
    s->c++; // update counter

}

void freeStackEl(typeOfStoredData *fe) {
    if (fe)
        free((stack_t *) fe);
}

typeOfStoredData pop(stack_t *s) {
    if (!s->top) {
        // return NULL;
    }
    typeOfStoredData *r = &s->top->data; // saves old top
    s->top = s->top->next;
    return *r;
}

bool sIsEmpty(stack_t *st) {
    return st->top;
}

stackMem_t *stackBotton(stack_t *s, stackMem_t **pLast) {
    if (!s->top) {
        return NULL;
    }
    stackMem_t *i = s->top;
    while (i->next != NULL) {
        if (i->next->next == NULL) {
            if (pLast != NULL)
                *pLast = i;
        }
        i = i->next;
    }
    return i;
}

typeOfStoredData popBack(stack_t *s) {
    stackMem_t *pLast;
    stackMem_t *bottonEl = stackBotton(s, &pLast);

    typeOfStoredData *r = &bottonEl->data; // returned el
    pLast->next = NULL;
    return *r;
}

void printMember(typeOfStoredData *data) {
    fprintf(stdout, "{%d %d}", data->a, data->b);
}

void printStack(stack_t *s, void (*printMem)(typeOfStoredData *)) {
    fprintf(stdout, "Your stack looks like: \n");
    stackMem_t *i = s->top;
    while (i->next != NULL) {
        printMem(&i->data);
        fprintf(stdout, " -> ");
        i = i->next;
    }
    printMember(&i->data);
}

