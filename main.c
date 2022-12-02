/**
 * @file main.c
 * @author Jan Brudný (xbrudn02@stud.fit.vut.cz)
 * @brief Hlavní soubor programu
 * Implementace překladače jazyka IFJ22
 */

#include "parser.h"
#include "lex.h"
#include <stdio.h>

int main() {
    headerCheck(stdin);
    return parser();
}