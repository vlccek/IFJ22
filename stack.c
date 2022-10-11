//
// Created by jvlk on 16.9.22.
//


#include "stack.h"

genericStack *gStackInit() {
    make_var(stack, genericStack *, sizeof(genericStack));
            stack->c = 0;
    stack->top = NULL;
    return stack;
}

//

int gStackPush(genericStack *s, void *nm) {
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

void *gStackPop(genericStack *s) {
    if (!s->top) {
        // return NULL;
    }

    if (s->top == NULL) {
        exit(ERR_RUNTIME);
    }

    void *r = s->top->data;// saves old top
    s->top = s->top->next;
    s->c--;
    return r;
}

bool gStackIsEmpty(genericStack *st) {
    if (st->top == NULL)
        return true;
    else
        return false;
}

stackMem_t *gStackBottom(genericStack *s, stackMem_t **pLast) {
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

void *gStackPopBack(genericStack *s) {
    stackMem_t *pLast;
    stackMem_t *bottomEl = gStackBottom(s, &pLast);

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

void *gStackTop(genericStack *s) {
    if (s->top == NULL)
        return NULL;
    else
        return s->top->data;
}


void gStackPrint(genericStack *s, void (*printMem)(void *)) {
    fprintf(stdout, "Your stack looks like: \n");
    stackMem_t *i = s->top;
    while (i->next != NULL) {
        printMem(i->data);
        fprintf(stdout, " -> ");
        i = i->next;
    }
    printMem(i->data);
    fprintf(stdout, "\n");
}
/*
 * Returns null when elemnt not exist
 */
void *gStackGetNth(genericStack *s, unsigned int numberForTop) {
    stackMem_t *i = s->top;
    while (numberForTop != 0 && s->c > numberForTop--) {
        i = i->next;
    }
    if (numberForTop != 0 || i == NULL) {
        return NULL;
    }

    return i->data;
}
/*
 * Returns null when elemnt not exist
 */
void *getFromTop(genericStack *s, unsigned int numberForTop) {
    stackMem_t *i = s->top;
    while (numberForTop != 0 && s->c > numberForTop--) {
        i = i->next;
    }
    if (numberForTop != 0) {
        return NULL;
    }

    return i->data;
}
/*
 * Returns null when elemnt not exist
 */
void *getFromTop(genericStack *s, unsigned int numberForTop) {
    stackMem_t *i = s->top;
    while (numberForTop != 0 && s->c > numberForTop--) {
        i = i->next;
    }
    if (numberForTop != 0) {
        return NULL;
    }

    return i->data;
}
/*
 * Returns null when elemnt not exist
 */
void *gStackGetNth(genericStack *s, unsigned int numberForTop) {
    stackMem_t *i = s->top;
    while (numberForTop != 0 && s->c > numberForTop--) {
        i = i->next;
    }
    if (numberForTop != 0) {
        return NULL;
    }

    return i->data;
}
