/**
 * @file main.c
 * @author Jan Brudný (xbrudn02@stud.fit.vut.cz)
 * @brief Hlavní soubor programu
 * Implementace překladače imperativního jazyka IFJ21
 * 
 * @version 0.1
 * @date 2022-11-20
 */

#include "parser.h"
#include "lex.h"
#include <stdio.h>

int main() {
    headerCheck(stdin);
    return parser();
}