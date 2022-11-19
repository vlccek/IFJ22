/**
 * @file dynstring.c
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @brief Jednoduchý dynamický string
 */


#include "dynstring.h"


dynStr_t *dstrInit() {
    make_var(str, dynStr_t*, sizeof(dynStr_t));
    if (str == NULL) {
        exit(ERR_RUNTIME);
    }


    str->string = (char *) malloc(sizeof(char) * default_str_size);
    if (str->string == NULL) {
        exit(ERR_RUNTIME);
    }
    str->string[0] = '\0';
    str->size = 0;
    str->maxSize = default_str_size;
    return str;
}

// calculate new size of string (2x)
static int newSize(int actualsize, int necessarySize) {
    int newstringsize = actualsize * 2;// expanding string size by 2
    if (newstringsize < necessarySize) {
        return newSize(newstringsize, necessarySize);// if it's not enought 2x
    } else {
        return newstringsize;
    }
}

// make string size bigger 2x
void dstrRealloc(dynStr_t *str, int necessarySize) {
    int newSz = newSize(str->maxSize, necessarySize);
    str->maxSize = newSz;
    char *tmp = realloc(str->string, newSz);
    if (tmp == NULL) {
        exit(ERR_RUNTIME);
    }
    str->string = tmp;
    str->string[str->size] = '\0';
}

void dstrAppend(dynStr_t *dstr, const char *t) {
    if (t == NULL) {
        return;
    }
    int appendStrSize = strlen(t);
    if (dstr->size + appendStrSize + 1 >= dstr->maxSize) {
        dstrRealloc(dstr, appendStrSize + 1); // \0 character +
    }
    dstr->size += appendStrSize;
    strcat(dstr->string, t);
}

char *dstrGet(dynStr_t *dstr) {
    return dstr->string;
}

sizeOfStr *dstrSize(dynStr_t *dstr) {
    return &dstr->size;
}

void dstrPrint(dynStr_t *dstr) {
    dstrfprint(dstr, stdout);
}

void dstrfprint(dynStr_t *dstr, FILE *fp) {
    fprintf(fp, "%s", dstr->string);
}

void dstrPrepend(dynStr_t *dstr, const char *newStr) {
    int appendStrSize = (int) strlen(newStr);
    if (dstr->size + appendStrSize + 1 >= dstr->maxSize) {
        // if size of concat strings is big then allocated reallocated new one
        dstrRealloc(dstr, appendStrSize + 1);
    }

    make_var(tmpStr, char *, strlen(dstr->string));
    strcpy(tmpStr, dstr->string);
    // saves original string
    tmpStr[dstr->size] = '\0';

    dstr->size += appendStrSize;

    strcpy(dstr->string, newStr);// copy prefix
    strcat(dstr->string, tmpStr);// concat sufix (original string)

    dstr->string[dstr->size] = '\0';
    free(tmpStr);
}

dynStr_t *dstrInitChar(const char *text) {
    dynStr_t *nstring = dstrInit();
    dstrAppend(nstring, text);
    return nstring;
}


dynStr_t *dstrSubstring(dynStr_t *dstr, int start, int stop) {
    if (start < 0 && stop == 0) {
        stop = dstr->size;
    }

    if (start > dstr->size) {
        start = dstr->size;
    }

    if (stop > dstr->size) {
        stop = dstr->size;
    }

    if (start < 0) {
        start = dstr->size + start;
    }
    if (stop < 0) {
        stop = dstr->size + stop;
    }
    if (start >= stop || start < 0 || stop < 0) {
        return dstrInit();
    }


    make_var(tmp, char *, stop - start + 1);
    strncpy(tmp, dstr->string + start, stop - start);
    tmp[stop - start] = '\0';

    dynStr_t *returnVal = dstrInitChar(tmp);

    free(tmp);
    return returnVal;
}

void dstrFree(dynStr_t *dstr) {

    free(dstr->string);
    free(dstr);
}