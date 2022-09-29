/**
 * @file lex.h
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Hlavičkový soubor pro lexikální analyzátor
 * Implementace překladače jazyka IFJ22
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

    //f curly braces
    curlyBraceRight,
    curlyBraceLeft,


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
} data_t;

// represents the output
typedef struct
{
    lexType type;
    data_t data;
    int rowNumber;
    int rowPosNumber;
} token_t;

// gets the next token and advances the pointer
token_t getToken(FILE *stream);

// TODO prints the content of a token
void printTokenData(token_t input);

// frees the token from memory
void freeToken(token_t *input);

/**
 * Resets row and rowPos counters.
 */
void resetCounters();

// sets the file pointer one token back
void ungetToken(FILE *stream);

#endif