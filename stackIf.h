/**
* @file stackIf.h
* @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
* @brief Wrapper pro stack.
 * Generates unique labels for jumps and memorizes state using stack
*/
#ifndef IFJ22_STACKIF_H
#define IFJ22_STACKIF_H
#include "dynstring.h"
#include "stack.h"
#include <string.h>

typedef enum {
    while_type,
    if_type,
} typeOfIfStackMem;

typedef struct {
    typeOfIfStackMem type;
    // for if
    bool inIfbranch;
    bool isEndingGenerated;
    bool expectinElse;
    bool inElse;

    //for while
    bool inWhileBody;
    bool endOfWhile;
    dynStr_t *endlabel; // ending label for label
    dynStr_t *elselabel;// label else label
} ifsState;

genericStack *ifS_Init();

// next if id  :) unique id
void ifS_newIf(genericStack *stack);
void ifS_newWhile(genericStack *stack);
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

typeOfIfStackMem ifs_getType(genericStack *stack);

void printifState(void *p);

#endif//IFJ22_STACKIF_H
