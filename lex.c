/**
 * @file lex.c
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Lexikální analyzátor
 * Implementace překladače jazyka IFJ22
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lex.h"
#include "common.h"

// row and row position counters
int row = 1;
int rowPos = 1;

// represents current position in current row
unsigned long position = 0;

// resets counters for row and row position
void resetCounters()
{
    row = 1;
    rowPos = 1;
}

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
    case floatLiteral:
        fprintf(stderr, "lextype: floatLiteral\n");
        fprintf(stderr, "value: %lf\n", input.data.valueFloat);
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
    case stringKey:
        fprintf(stderr, "lextype: stringKey\n");
        break;
    case floatKey:
        fprintf(stderr, "lextype: floatKey\n");
        break;
    case intKey:
        fprintf(stderr, "lextype: intKey\n");
        break;
    case stringNullKey:
        fprintf(stderr, "lextype: stringNullKey\n");
        break;
    case floatNullKey:
        fprintf(stderr, "lextype: floatNullKey\n");
        break;
    case intNullKey:
        fprintf(stderr, "lextype: intNullKey\n");
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

    fprintf(stderr, "row: %d\nrow position: %d\n", input.rowNumber, input.rowPosNumber);

    return;
}

// frees the token from memory
void freeToken(token_t *input)
{
    switch (input->type)
    {
    case stringLiteral:
    case identifierFunc:
    case identifierVar:
        dstrFree(input->data.valueString);
        break;
    case floatLiteral:
        input->data.valueFloat = 0;
        break;
    case integerLiteral:
        input->data.valueInteger = 0;
        break;
    default:
        break;
    }
    return;
}

// TODO edge cases, finish the function
void incrementCounters(char c)
{
    switch (c)
    {
        // the alphabet
        case 'a' ... 'z':
        case 'A' ... 'Z':
            rowPos++;
            break;
        // whitespace characters
        case ' ':
            rowPos++;
            break;
        case '\n':
            row++;
            rowPos = 1;
            break;
        case '\t':
        case '\r':
        case '\v':
        case '\f':
            break;
        // default
        default:
            rowPos++;
            break;
    }
    return;
}

// TODO do we need it? finish the function
// TODO custom ungetc
void setBack(FILE *stream, int symbol)
{
    ungetc(symbol, stream);
    rowPos--;
}

// sets the file pointer one token back
void ungetToken(FILE *stream)
{
    fseek(stream, position, SEEK_SET);
}

// gets the next token and advances the pointer TODO
token_t getToken(FILE *stream)
{
    // initial declarations
    state currentState = init_s;
    token_t outputToken;

    outputToken.rowNumber = row;
    outputToken.rowPosNumber = rowPos;

    fflush(stream);
    position = ftell(stream);

    int charCounter = 0; // or 1? TODO
    int currentChar = getc(stream);

    int bufferLevel = 1;
    bool stop = false;
    int commentCounter = 0;

    // sets up a buffer for saving chars inside a literal or identifier names
    char *buffer = malloc(sizeof(char) * 100 * bufferLevel);
    if (buffer == NULL)
    {
        InternalError("Memory allocation failed.");
    }

    // EOF token
    if (currentChar == EOF)
    {
        outputToken.type = ending;
        return outputToken;
    }

    // parsing loop and FSM
    while ((currentChar != EOF) && (stop != true))
    {
        // FSM
        switch (currentChar)
        {
            // the alphabet
            // TODO e for float literal
            case 'a' ... 'z':
            case 'A' ... 'Z':
                switch (currentState)
                {
                    case init_s:
                    case identifier_func_f_s:
                        currentState = identifier_func_f_s;
                        break;
                    case identifier_var_dollar_s:
                    case identifier_var_f_s:
                        currentState = identifier_var_f_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            // TODO
            case ' ':
                switch (currentState)
                {
                    case init_s:
                        outputToken.rowPosNumber++;
                        currentState = init_s;
                    break;
                    // TODO
                    default:
                        stop = true;
                        break;
                }
            case '\n':
                switch (currentState)
                {
                    case init_s:
                        currentState = init_s;
                        break;
                    // TODO
                    default:
                        stop = true;
                        break;
                }
            case '\t':
            case '\r':
            case '\v':
            case '\f':
                break;
        }
        // increments the position
        //fprintf(stderr, "current char %c: incrementing from %d to %d\n", currentChar, rowPos, rowPos + 1); // TODO testing
        fprintf(stderr, "current char = %c\n", currentChar); // TODO testing
        incrementCounters(currentChar);
        currentChar = getc(stream);

    }

    // returns the output token
    return outputToken;
}