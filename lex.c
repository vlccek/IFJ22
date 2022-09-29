/**
 * @file lex.c
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Lexikální analyzátor
 * Implementace překladače jazyka IFJ22
 */

#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

void printTokenData(token_t input)
{
    switch (input.type)
    {
    // literals
    case stringLiteral:
        fprintf(stderr, "lextype: stringLiteral\n");
        fprintf(stderr, "value: %s\n", input.data.valueString);
        break;
    default:
        // TODO
        break;
    return;
    }
}