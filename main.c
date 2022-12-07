/**
 * @file main.c
 * @author Jan Brudný (xbrudn02@stud.fit.vut.cz)
 * @brief Main
 * Implementation IFJ22 compiler
 */

#include "parser.h"
#include "lex.h"
#include <stdio.h>

int main() {
    headerCheck(stdin);
    return parser();
}