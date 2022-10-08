/**
 * @file dynstring.h
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @brief Jednoduchý dynamický string
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

#ifndef STACK_DYNSTRING_H
#define STACK_DYNSTRING_H

// errs
#define default_str_size 64

typedef int sizeOfStr;

struct dynStr {
    char *string;
    sizeOfStr size;
    sizeOfStr maxSize;
};

typedef struct dynStr dynStr_t;

/**
 * Inicalizaton
 * @return incalized string
 */
dynStr_t *dstrInit();

/**
 * Returns pointer to char
 * @param dstr
 * @return
 */
char *dstrGet(dynStr_t *dstr);

sizeOfStr *dstrSize(dynStr_t *dstr);

/**
 * Insert text behind last character
 * @param dstr
 * @return
 */

void dstrPrint(dynStr_t *dstr);

void dstrfprint(dynStr_t *dstr, FILE *fp);

static int newSize(int actualsize, int necessarySize);

static void dstrRealloc(dynStr_t *str, int necessarySize);

void dstrAppend(dynStr_t *dstr, char *t);

dynStr_t *dstrInitChar(const char *text) ;
void dstrPrepend(dynStr_t *dstr, char *newStr);

inline bool dstrCmp(dynStr_t *dstr, dynStr_t *dstr2) {
    return strcmp(dstr->string, dstr2->string) == 0 ? true : false;
}

inline int dstrLen(dynStr_t *dstr) {
    return dstr->size;
}

dynStr_t *dstrSubstring(dynStr_t *dstr, int start, int stop);

void dstrFree(dynStr_t *dstr);

#endif //STACK_DYNSTRING_H
