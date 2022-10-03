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
        fprintf(stderr, "lextype: elseKey\n");
        break;
    case functionKey:
        fprintf(stderr, "lextype: functionkey\n");
        break;
    case ifKey:
        fprintf(stderr, "lextype: ifKey\n");
        break;
    case nullKey:
        fprintf(stderr, "lextype: nullKey\n");
        break;
    case returnKey:
        fprintf(stderr, "lextype: returnKey\n");
        break;
    case voidKey:
        fprintf(stderr, "lextype: voidKey\n");
        break;
    case whileKey:
        fprintf(stderr, "lextype: whileKey\n");
        break;
    // curly braces
    case curlyBraceRight:
        fprintf(stderr, "lextype: curlyBraceRight\n");
        break;
    case curlyBraceLeft:
        fprintf(stderr, "lextype: curlyBraceLeft\n");
        break;
    // data types
    case stringDat:
        fprintf(stderr, "lextype: stringDat\n");
        break;
    case floatDat:
        fprintf(stderr, "lextype: floatDat\n");
        break;
    case intDat:
        fprintf(stderr, "lextype: intDat\n");
        break;
    case stringNullDat:
        fprintf(stderr, "lextype: stringNullDat\n");
        break;
    case floatNullDat:
        fprintf(stderr, "lextype: floatNullDat\n");
        break;
    case intNullDat:
        fprintf(stderr, "lextype: intNullDat\n");
        break;
    // assignment
    case equals:
        fprintf(stderr, "lextype: equals\n");
        break;
    case colon:
        fprintf(stderr, "lextype: colon\n");
        break;
    case semicolon:
        fprintf(stderr, "lextype: semicolon\n");
        break;
    case comma:
        fprintf(stderr, "lextype: comma\n");
        break;
    // operators
    case multiplicationOp:
        fprintf(stderr, "lextype: multiplicationOp\n");
        break;
    case divisionOp:
        fprintf(stderr, "lextype: divisionOp\n");
        break;
    case plusOp:
        fprintf(stderr, "lextype: plusOp\n");
        break;
    case minusOp:
        fprintf(stderr, "lextype: minusOp\n");
        break;
    case concatenationOp:
        fprintf(stderr, "lextype: concatenationOp\n");
        break;
    case lesserThanOp:
        fprintf(stderr, "lextype: lesserThanOp\n");
        break;
    case lesserEqOp:
        fprintf(stderr, "lextype: lesserEqOp\n");
        break;
    case greaterThanOp:
        fprintf(stderr, "lextype: greaterThanOp\n");
        break;
    case greaterEqOp:
        fprintf(stderr, "lextype: greaterEqOp\n");
        break;
    case eqOp:
        fprintf(stderr, "lextype: eqOp\n");
        break;
    case notEqOp:
        fprintf(stderr, "lextype: notEqOp\n");
        break;
    // parentheses
    case leftPar:
        fprintf(stderr, "lextype: leftPar\n");
        break;
    case rightPar:
        fprintf(stderr, "lextype: rightPar\n");
        break;
    // identifiers
    case identifierFunc:
        fprintf(stderr, "lextype: identifierFunc\n");
        fprintf(stderr, "value: %s\n", dstrGet(input.data.valueString));
        break;
    case identifierVar:
        fprintf(stderr, "lextype: identifierVar\n");
        fprintf(stderr, "value: %s\n", dstrGet(input.data.valueString));
        break;
    // ending
    case ending:
        fprintf(stderr, "lextype: ending\n");
        break;
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
    // for testing purposes
    token_t token;
    token.type = unknown;
    token.data.valueString = dstrInit();
    return token;
}