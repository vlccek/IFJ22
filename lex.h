/**
 * @file lex.h
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Hlavičkový soubor pro lexikální analyzátor
 * Implementace překladače imperativního jazyka IFJ21
 */

#include <stdio.h>
#include <stdlib.h>
#include "dynstring.h"

#ifndef LUAINTERPRET_LEX_H
#define LUAINTERPRET_LEX_H
// represents the type of lexical item
typedef enum
{
    // keywords
    elseKey,
    functionKey,
    ifKey,
    nullKey,
    returnKey,
    voidKey,
    whileKey,

    // data types
    stringKey, // "'string' $hello"
    floatKey,
    intKey,
    stringNullKey,
    floatNullKey,
    intNullKey,

    // assignment
    equals,
    colon,
    semiColon,
    comma,

    // operators
    multiplicationOp,
    divisionOp,
    plusOp,
    minusOp,
    concatenationOp,
    lesserThanOp,
    lesserEqOp,
    greaterThanOp,
    greaterEqOp,
    eqOp,
    notEqOp,

    // parentheses
    leftPar,
    rightPar,

    // literals
    stringLiteral, // "hello"
    integerLiteral,
    decimalLiteral,

    // identifiers
    identifierFce,
    identifierVar,

    // unknown
    unknown,

    // EOF type
    ending,
    lexTypeCount
} lexType;

// represents the value of the token
typedef union
{
    char *valueString;
    int valueInteger;
    double valueNumber;
} Data;

// represents the output
typedef struct
{
    lexType type;
    Data data;
    int rowNumber;
    int rowPosNumber;
} token;

// gets the next token and advances the pointer
token getToken(FILE *stream);

// TODO prints the content of a token
void printTokenData(token input);

// frees the token from memory
void freeToken(token *input);

/**
 * Resets row and rowPos counters.
 */
void resetCounters();

// sets the file pointer one token back
void ungetToken(FILE *stream);

#endif
