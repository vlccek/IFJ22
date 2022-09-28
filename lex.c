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

int main()
{
    // testing
    token_t token;
    data_t data;

    token.type = stringLiteral;
    data.valueString = "Hello World";
    token.data = data;
    token.rowNumber = 1;
    token.rowPosNumber = 1;

    printTokenData(token);
    
    return 0;
}