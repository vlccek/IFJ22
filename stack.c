//
// Created by jvlk on 16.9.22.
//



#include "stack.h"

stack_type *stackInit() {
    make_var(stack, stack_type*, sizeof(stack_type))
    stack->c = 0;
    stack->top = NULL;
    return stack;
}

//

int push(stack_type *s, void *nm) {
    if (!s || !nm) {
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

void *pop(stack_type *s) {
    if (!s->top) {
        // return NULL;
    }
    void *r = s->top->data; // saves old top
    s->top = s->top->next;
    return r;
}

bool sIsEmpty(stack_type *st) {
    return st->top;
}

stackMem_t *stackBottom(stack_type *s, stackMem_t **pLast) {
    if (!s->top) {
        return NULL;
    }
    if (s->top->next == NULL) {
        *pLast = NULL;
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

void *popBack(stack_type *s) {
    stackMem_t *pLast;
    stackMem_t *bottomEl = stackBottom(s, &pLast);


    void *r = bottomEl->data; // returned el

    // last memeber check
    if (pLast == NULL) {
        s->top = NULL;
    } else {
        pLast->next = NULL;
    }

    return r;
}


void printStack(stack_type *s, void (*printMem)(void *)) {
    fprintf(stdout, "Your stack looks like: \n");
    stackMem_t *i = s->top;
    while (i->next != NULL) {
        printMem(&i->data);
        fprintf(stdout, " -> ");
        i = i->next;
    }
    printMem(&i->data);
}
