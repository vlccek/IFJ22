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
    stringKey,
    floatKey,
    intKey,
    stringNullKey,
    floatNullKey,
    intNullKey,

    // curly braces
    curlyBraceRight,
    curlyBraceLeft,

    // assignment
    equals,
    colon,
    semicolon,
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
    floatLiteral,

    // identifiers
    identifierFunc,
    identifierVar,

    // unknown
    unknown,

    // EOF type
    ending
} lexType;

// FSM states
typedef enum
{
    // initial state
    init_s,

    // literal states
    string_lit_s,
    string_lit_f_s,
    integer_lit_f_s,
    float_lit_dot_s,
    float_lit_e_s,
    float_lit_sign_s,
    float_lit_exp_f_s,
    float_lit_f_s,

    // identifier states
    identifier_func_f_s,
    identifier_var_dollar_s,
    identifier_var_f_s,

    // operator states
    multiplication_f_s,
    division_f_s,
    plus_f_s,
    minus_f_s,
    dot_f_s,
    concatenation_f_s,
    lesser_than_f_s,
    lesser_eq_f_s,
    greater_than_f_s,
    greater_eq_f_s,
    eq_1_s,
    eq_2_s,
    eq_f_s,
    not_eq_1_s,
    not_eq_2_s,
    not_eq_f_s,

    // TODO

    // nullable type state
    null_f_s,

    // unknown state
    unknown_f_s
} state;

// represents the value of the token
typedef union
{
    dynStr_t *valueString;
    int valueInteger;
    double valueFloat;
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

// prints the content of a token
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