/**
 * @file lex.c
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Lexikální analyzátor
 * Implementace překladače jazyka IFJ22
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lex.h"
#include "common.h"

// row and row position counters
int row = 1;
int rowPos = 0;

// header conditions
bool php = false;
bool declare = false;

// represents current position in current row
unsigned long position = 0;

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
            rowPos = 0;
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

// TODO edge cases, finish the function
void decrementCounters(char c)
{
    switch (c)
    {
        // the alphabet
        case 'a' ... 'z':
        case 'A' ... 'Z':
            rowPos--;
            break;
        // whitespace characters
        case ' ':
            rowPos--;
            break;
        case '\n':
            row--;
            // TODO rowPos = 0;
            break;
        case '\t':
        case '\r':
        case '\v':
        case '\f':
            break;
        // default
        default:
            rowPos--;
            break;
    }
    return;
}

// gets next char and automatically increments counters
int getNextChar(FILE *stream)
{
    int outputChar = getc(stream);
    incrementCounters(outputChar);
    return outputChar;
}

// TODO ungets next char and automatically decrements counters
void ungetNextChar(FILE *stream, int currentChar)
{
    ungetc(currentChar, stream);
    decrementCounters(currentChar);
    return;
}

// sets the file pointer one token back TODO testing! could potentially cause problems due to different position calculating
void ungetToken(FILE *stream)
{
    fseek(stream, position, SEEK_SET);
}

// reallocs the buffer if needed and then writes in the buffer
void writeToBuffer(int *charCounter, int *bufferLevel, char *buffer, int currentChar)
{
    // temporary variables
    int charCounterTemp = *charCounter;
    int bufferLevelTemp = *bufferLevel;

    // reallocs the buffer if needed
    if (charCounterTemp > ((100 * bufferLevelTemp) - 2))
    {
        buffer = realloc(buffer, sizeof(char) * 100 * ++bufferLevelTemp);
        if (buffer == NULL)
        {
            InternalError("Memory allocation failed.");
        }
    }

    // writes to the buffer
    buffer[charCounterTemp] = currentChar;
    charCounterTemp++;

    // updates values pointed at by pointers
    *charCounter = charCounterTemp;
    *bufferLevel = bufferLevelTemp;

    return;
}

// checks if the "<?php" part of the header is present and correct
void headerCheck(FILE *stream)
{
    // php check
    if (php == false)
    {
        char buffer[100];
        fgets(buffer, 100, stream);
        if ((strcmp(buffer, "<?php") == 0) || (strcmp(buffer, "<?php\n") == 0) || (strcmp(buffer, "<?php\r\n") == 0))
        {
            php = true;
            row++;
        }
        else
        {
            fprintf(stderr, "Syntax error: incorrect header!\n");
            exit(2);
        }
    }
    // declare check
    if (declare == false)
    {
        // TODO
    }
}

// gets the next token and advances the pointer TODO
token_t getToken(FILE *stream)
{
    // header check, first part
    headerCheck(stream);

    // initial declarations
    state currentState = init_s;
    token_t outputToken;

    outputToken.rowNumber = 0;
    outputToken.rowPosNumber = 0;

    fflush(stream);
    position = ftell(stream);

    int charCounter = 0;
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
    else
    {
        ungetc(currentChar, stream);
    }

    // parsing loop and FSM
    while (stop != true)
    {
        // gets next char and increments counters
        currentChar = getNextChar(stream);

        // buffer switch
        bool bufferOn = false;

        // checking for EOF
        if (currentChar == EOF)
        {
            break;
        }

        // checking if the current state is the initial state
        // (row position of token is set when transitioning from init_s to a different state)
        bool isInit = false;
        if (currentState == init_s)
        {
            isInit = true;
        }

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
                        bufferOn = true;
                        currentState = identifier_func_f_s;
                        break;
                    case identifier_var_f_s:
                    case identifier_var_dollar_s:
                        bufferOn = true;
                        currentState = identifier_var_f_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case integer_lit_f_s:
                    case float_lit_f_s:
                        if ((currentChar == 'e') || (currentChar == 'E'))
                        {
                            bufferOn = true;
                            currentState = float_lit_e_s;
                        }
                        else
                        {
                            currentState = unknown_f_s;
                        }
                        break;
                    case null_f_s:
                        bufferOn = true;
                        currentState = null_f_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            // numbers
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                switch (currentState)
                {
                    case init_s:
                    case integer_lit_f_s:
                        bufferOn = true;
                        currentState = integer_lit_f_s;
                        break;
                    case float_lit_dot_s:
                    case float_lit_f_s:
                        bufferOn = true;
                        currentState = float_lit_f_s;
                        break;
                    case float_lit_e_s:
                    case float_lit_exp_f_s:
                    case float_lit_sign_s:
                        bufferOn = true;
                        currentState = float_lit_exp_f_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case identifier_func_f_s:
                        bufferOn = true;
                        currentState = identifier_func_f_s;
                        break;
                    case identifier_var_f_s:
                        bufferOn = true;
                        currentState = identifier_var_f_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            // TODO special characters
            case '(':
                switch (currentState)
                {
                    case init_s:
                        currentState = left_par_f_s;
                        stop = true;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case identifier_func_f_s:
                        ungetNextChar(stream, currentChar),
                        currentState = identifier_func_f_s;
                        stop = true;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case ')':
                switch (currentState)
                {
                    case init_s:
                        currentState = right_par_f_s;
                        stop = true;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '$':
                switch (currentState)
                {
                    case init_s:
                        currentState = identifier_var_dollar_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '_':
                switch (currentState)
                {
                    case init_s:
                        bufferOn = true;
                        currentState = identifier_func_f_s;
                        break;
                    case identifier_var_dollar_s:
                    case identifier_var_f_s:
                        bufferOn = true;
                        currentState = identifier_var_f_s;
                        break;
                    case identifier_func_f_s:
                        bufferOn = true;
                        currentState = identifier_func_f_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '?':
                switch (currentState)
                {
                    case init_s:
                        currentState = null_f_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '\"':
                switch (currentState)
                {
                    case init_s:
                        currentState = string_lit_s;
                        break;
                    case string_lit_s:
                        currentState = string_lit_f_s;
                        stop = true;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '.':
                switch (currentState)
                {
                    case init_s:
                        currentState = dot_f_s;
                        stop = true;
                        break;
                    case integer_lit_f_s:
                        bufferOn = true;
                        currentState = float_lit_dot_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '+':
            case '-':
                switch (currentState)
                {
                    case init_s:
                        currentState = plus_f_s;
                        stop = true;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case float_lit_e_s:
                        bufferOn = true;
                        currentState = float_lit_sign_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '/':
                switch (currentState)
                {
                    case init_s:
                        currentState = com_slash_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_slash_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_ast_s:
                        currentState = init_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            case '*':
                switch (currentState)
                {
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_slash_s:
                        currentState = com_block_s;
                        break;
                    case com_block_s:
                        currentState = com_block_ast_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
            // TODO whitespaces
            case ' ':
                switch (currentState)
                {
                    case init_s:
                        currentState = init_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    // TODO
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        stop = true;
                        break;
                }
                break;
            case '\n':
                switch (currentState)
                {
                    case init_s:
                        currentState = init_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    // TODO
                    case com_line_f_s:
                        currentState = init_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        stop = true;
                        break;
                }
                break;
            case '\t':
            case '\r':
            case '\v':
            case '\f':
                break;
            default:
                switch (currentState)
                {
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    default:
                        currentState = unknown_f_s;
                        break;
                }
                break;
        }
        // if current state changed from init_s to something else, sets token position
        if (isInit)
        {
            if (currentState != init_s)
            {
                // sets the position
                outputToken.rowNumber = row;
                outputToken.rowPosNumber = rowPos;
            }
        }

        // writes to buffer if the buffer switch is on
        if (bufferOn)
        {
            writeToBuffer(&charCounter, &bufferLevel, buffer, currentChar);
        }
    }

    // TODO assigns correct lexType to the output token
    switch (currentState)
    {
        // literal states
        case integer_lit_f_s:
            outputToken.data.valueInteger = atoi(buffer);
            outputToken.type = integerLiteral;
            break;
        case float_lit_f_s:
        case float_lit_exp_f_s:
            outputToken.data.valueFloat = strtod(buffer, NULL);
            outputToken.type = floatLiteral;
            break;
        case string_lit_f_s:
            outputToken.data.valueString = dstrInitChar(buffer);
            outputToken.type = stringLiteral;
            break;
        // identifier states
        case identifier_func_f_s:
            outputToken.data.valueString = dstrInitChar(buffer);
            if (strcmp(buffer, "else") == 0)
            {
                outputToken.type = elseKey;
            }
            else if (strcmp(buffer, "function") == 0)
            {
                outputToken.type = functionKey;
            }
            else if (strcmp(buffer, "if") == 0)
            {
                outputToken.type = ifKey;
            }
            else if (strcmp(buffer, "null") == 0)
            {
                outputToken.type = nullKey;
            }
            else if (strcmp(buffer, "return") == 0)
            {
                outputToken.type = returnKey;
            }
            else if (strcmp(buffer, "void") == 0)
            {
                outputToken.type = voidKey;
            }
            else if (strcmp(buffer, "while") == 0)
            {
                outputToken.type = whileKey;
            }
            else if (strcmp(buffer, "string") == 0)
            {
                outputToken.type = stringKey;
            }
            else if (strcmp(buffer, "float") == 0)
            {
                outputToken.type = floatKey;
            }
            else if (strcmp(buffer, "int") == 0)
            {
                outputToken.type = intKey;
            }                                                                                                                      
            else
            {
                outputToken.type = identifierFunc;
            }
            break;
        case identifier_var_f_s:
            outputToken.data.valueString = dstrInitChar(buffer);
            outputToken.type = identifierVar;
            break;
        // null type state
        case null_f_s:
            outputToken.data.valueString = dstrInitChar(buffer);
            if (strcmp(buffer, "string") == 0)
            {
                outputToken.type = stringNullKey;
            }
            else if (strcmp(buffer, "float") == 0)
            {
                outputToken.type = floatNullKey;
            }
            else if (strcmp(buffer, "int") == 0)
            {
                outputToken.type = intNullKey;
            }
            else
            {
                outputToken.type = unknown;
            }
            break;
        // parentheses states
        case left_par_f_s:
            outputToken.type = leftPar;
            break;
        case right_par_f_s:
            outputToken.type = rightPar;
            break;
        // commentary state
        case com_line_f_s:
            outputToken.type = ending;
            break;
        // init state (when stream ends with block commentary)
        case init_s:
            outputToken.type = ending;
            break;
        // unknown state and default
        case unknown_f_s:
        default:
            fprintf(stderr, "Lexical error on ln %d, col %d!\n", outputToken.rowNumber, outputToken.rowPosNumber);
            exit(1);
            break;
    }

    // returns the output token
    return outputToken;
}