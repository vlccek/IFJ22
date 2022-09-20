//
// Created by jvlk on 20.9.22.
//

#include "stack.c"

int main(int argc, char *argv[]) {

    stack_t *st = stack();

    struct ints2 a = {10, 20};
    struct ints2 b = {11, 20};
    struct ints2 c = {10, 21};

    push(st, a);
    push(st, b);
    push(st, c);
    printStack(st, printMember);


    typeOfStoredData tmp = popBack(st);
    //  freeStackEl(tmp);


    return 0;

}