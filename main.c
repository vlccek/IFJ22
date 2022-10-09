//
// Created by jvlk on 20.9.22.
//

#include "stack.h"
#include "dynstring.h"

int main(int argc, char *argv[]) {

    dynStr_t *dstr = dstrInit();
    char *tmp = "Tvojem mamam!";

    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);
    dstrAppend(dstr, tmp);

    char *a = dstrGet(dstr);

    dstrPrint(dstr);

    dstrFree(dstr);
    return 0;

}