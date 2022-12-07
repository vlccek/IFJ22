//
// Created by jvlk on 6.12.22.
//

#ifndef IFJ22_STACKIF_H
#define IFJ22_STACKIF_H
#include "dynstring.h"
#include "stack.h"
#include <string.h>

typedef struct {
    bool inIfbranch;
    bool isEndingGenerated;
    bool expectinElse;
    bool inElse;
    dynStr_t *endlabel; // ending label for label
    dynStr_t *elselabel;// label else label
} ifsState;

genericStack *ifS_Init();

// next if id  :) unique id
void ifS_new(genericStack *stack);
void ifS_old(genericStack *stack);

dynStr_t *ifS_ending(genericStack *stack);
dynStr_t *ifS_else(genericStack *stack);
bool ifS_isEndingGenerated(genericStack *stack);
void ifS_isSetEndingGenerated(genericStack *stack, bool end);

bool ifS_expectingElse(genericStack *stack);
void ifS_SetExpectingElse(genericStack *stack, bool end);

bool ifS_inElse(genericStack *stack);
void ifS_SetsinElse(genericStack *stack, bool end);

bool ifS_inIfbranch(genericStack *stack);
void ifS_SetinIfbranch(genericStack *stack, bool end);

bool ifS_isEmpty(genericStack *stack);

void printifState(void *p);

#endif//IFJ22_STACKIF_H
