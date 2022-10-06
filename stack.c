//
// Created by jvlk on 16.9.22.
//


#include "stack.h"

genericStack *stackInit() {
    make_var(stack, genericStack *, sizeof(genericStack))
            stack->c = 0;
    stack->top = NULL;
    return stack;
}

//

int push(genericStack *s, void *nm) {
    if (!s || !nm) {
        //Stack is null
        return ERR_RUNTIME;
    }

    stackMem_t *oldtop = s->top;// saves old top

    make_var(newtop, stackMem_t *, sizeof(stackMem_t))
            s->top = newtop;

    s->top->data = nm;    // set data of new top
    s->top->next = oldtop;// point from old top to new top
    ///////////////////////// could be null!!!!
    s->c++;// update counter
    return 0;
}

void *pop(genericStack *s) {
    if (!s->top) {
        // return NULL;
    }

    if (s->top == NULL) {
        exit(ERR_RUNTIME);
    }

    void *r = s->top->data;// saves old top
    s->top = s->top->next;
    s--;
    return r;
}

bool sIsEmpty(genericStack *st) {
    if (st->top == NULL)
        return true;
    else
        return false;
}

stackMem_t *stackBottom(genericStack *s, stackMem_t **pLast) {
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

void *popBack(genericStack *s) {
    stackMem_t *pLast;
    stackMem_t *bottomEl = stackBottom(s, &pLast);

    if (s->top == NULL){
        exit(ERR_RUNTIME);
    }

    void *r = bottomEl->data;// returned el

    // last memeber check
    if (pLast == NULL) {
        s->top = NULL;
    } else {
        pLast->next = NULL;
    }
    s->c--;

    return r;
}

void *stackTop(genericStack *s) {
    if (s->top == NULL)
        exit(ERR_RUNTIME);
    else
        return s->top->data;
}


void printStack(genericStack *s, void (*printMem)(void *)) {
    fprintf(stdout, "Your stack looks like: \n");
    stackMem_t *i = s->top;
    while (i->next != NULL) {
        printMem(&i->data);
        fprintf(stdout, " -> ");
        i = i->next;
    }
    printMem(&i->data);
}
