/**
 * @file lex.c
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Lexikální analyzátor
 * Implementace překladače jazyka IFJ22
 */

#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

// prints the content of a token
void printTokenData(token_t input)
{
    switch (input.type)
    {
    // literals
    case stringLiteral:
        fprintf(stderr, "lextype: stringLiteral\n");
        fprintf(stderr, "value: %s\n", dstrGet(input.data.valueString));
        break;
    case integerLiteral:
        fprintf(stderr, "lextype: integerLiteral\n");
        fprintf(stderr, "value: %d\n", input.data.valueInteger);
        break;
    case decimalLiteral:
        fprintf(stderr, "lextype: decimalLiteral\n");
        fprintf(stderr, "value: %lf\n", input.data.valueNumber);
        break;
    // keywords
    case elseKey:
    case functionKey:
    case ifKey:
    case nullKey:
    case returnKey:
    case voidKey:
    case whileKey:
    // curly braces
    case curlyBraceRight:
    case curlyBraceLeft:
    // data types
    case stringDat:
    case floatDat:
    case intDat:
    case stringNullDat:
    case floatNullDat:
    case intNullDat:
    // assignment
    case equals:
    case colon:
    case semiColon:
    case comma:
    // operators
    case multiplicationOp:
    case divisionOp:
    case plusOp:
    case minusOp:
    case concatenationOp:
    case lesserThanOp:
    case lesserEqOp:
    case greaterThanOp:
    case greaterEqOp:
    case eqOp:
    case notEqOp:
    // parentheses
    case leftPar:
    case rightPar:
    // identifiers
    case identifierFunc:
    case identifierVar:
    // ending
    case ending:
    // unknown & default
    case unknown:
    default:
        fprintf(stderr, "lextype: unknown\n");
        break;
    }

    fprintf(stderr, "row: %d\nrow position: %d\n\n", input.rowNumber, input.rowPosNumber);

    return;
}

// TODO
token_t getToken(FILE *stream)
{
    token_t token;
    token.type = unknown;
    return token;
}