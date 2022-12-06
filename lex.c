/**
 * @file lex.c
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Lexikální analyzátor
 * Implementace překladače jazyka IFJ22
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "common.h"

// logging switch
#define lexLog false

// for tests
token_t *testTokens;
bool teston;

// row and row position counters
int row = 1;
int rowPos = 0;

// header conditions
bool php = false;
bool declare = false;

// ending mark check
bool endingMark = false;

// buffer for the last token
token_t lastToken;
// should the last token be returned or new one parsed from input
bool useLastToken = false;

// prints the content of a token
void printTokenData(token_t input) {
    switch (input.type) {
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
void freeToken(token_t *input) {
    switch (input->type) {
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
void incrementCounters(int c) {
    switch (c) {
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
            rowPos += 4;
            break;
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
void decrementCounters(int c) {
    switch (c) {
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
int getNextChar(FILE *stream) {
    int outputChar = getc(stream);
    incrementCounters(outputChar);
    if (lexLog)
        loging("Next char: %c %d", outputChar, outputChar);// TODO
    return outputChar;
}

// TODO ungets next char and automatically decrements counters
void ungetNextChar(FILE *stream, int currentChar) {
    ungetc(currentChar, stream);
    decrementCounters(currentChar);
    return;
}

// tell getToken to just return the last token and not parse the next one
void ungetToken(FILE *stream) {
    loging("Ungetting token");
    if (useLastToken)
    {
        InternalError("ungetToken called twice in a row");
    }
    useLastToken = true;
    
}

// writes in dynamic string serving as a buffer
void writeToBuffer(dynStr_t *string, int currentChar) {
    char tmp[2] = {(char) currentChar, 0};
    dstrAppend(string, tmp);
    return;
}

// checks if the "<?php" part of the header is present and correct
void headerCheck(FILE *stream) {
    // php check
    if (php == false) {
        char buffer[100];
        fgets(buffer, 100, stream);
        if ((strcmp(buffer, "<?php") == 0) || (strcmp(buffer, "<?php\n") == 0) || (strcmp(buffer, "<?php\r\n") == 0)) {
            php = true;
            row++;
        } else {
            fprintf(stderr, "Syntax error: incorrect header!\n");
            exit(2);
        }
    }
    // declare check
    if (declare == false) {
        token_t checkToken = getToken(stream);
        if (checkToken.type == declareHeader) {
            char str[18];
            fgets(str, 18, stream);
            if (strcmp(str, "(strict_types=1);") == 0) {
                declare = true;
                return;
            } else {
                fprintf(stderr, "Syntax error: incorrect header!\n");
                exit(2);
            }
        } else {
            fprintf(stderr, "Syntax error: incorrect header!\n");
            exit(2);
        }
    }
}

void flushEscSeqBuffer(dynStr_t *buffer, dynStr_t *escSeqBuffer) {
    dstrAppend(buffer, dstrGet(escSeqBuffer));
    clearBuffer(escSeqBuffer);
    return;
}

void clearBuffer(dynStr_t *buffer) {
    dstrFree(buffer);
    buffer = dstrInit();
}

// gets the next token and advances the pointer TODO
token_t getToken(FILE *stream) {

    if (useLastToken) {
        useLastToken = false;
        return lastToken;
    }
    
    // initial declarations
    state currentState = init_s;
    token_t outputToken;

    outputToken.rowNumber = 0;
    outputToken.rowPosNumber = 0;

    fflush(stream);

    int currentChar = getc(stream);
    if (lexLog)
        loging("Next char: %c", currentChar);// TODO

    bool stop = false;
    int commentCounter = 0;

    // sets up a dynamic string (buffer) for saving chars inside a literal or identifier names
    dynStr_t *buffer = dstrInit();

    // sets up a dynamic string (buffer) for saving chars while in a potential escape sequence
    dynStr_t *escSeqBuffer = dstrInit();

    // EOF token
    if (currentChar == EOF) {
        outputToken.type = ending;
        lastToken = outputToken;
        return outputToken;
    } else {
        ungetc(currentChar, stream);
    }

    // parsing loop and FSM
    while (stop != true) {
        // gets next char and increments counters
        currentChar = getNextChar(stream);
        // buffer switches
        bool bufferOn = false;
        bool escBufferOn = false;

        // checking for EOF
        if (currentChar == EOF) {
            if (lexLog)
                loging("Found EOF");
            break;
        }

        // checking if the current state is the initial state
        // (row position of token is set when transitioning from init_s to a different state)
        bool isInit = false;
        if (currentState == init_s) {
            isInit = true;
        }

        // FSM
        switch (currentChar) {
            // the alphabet
            // TODO e for float literal
            case 'a' ... 'z':
            case 'A' ... 'Z':
                switch (currentState) {
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
                        if ((currentChar == 'e') || (currentChar == 'E')) {
                            bufferOn = true;
                            currentState = float_lit_e_s;
                        } else {
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
                    case string_lit_backslash_s:
                        if (currentChar == 'n') {
                            writeToBuffer(buffer, '\n');
                            currentState = string_lit_s;
                        } else if (currentChar == 't') {
                            writeToBuffer(buffer, '\t');
                            currentState = string_lit_s;
                        } else if (currentChar == 'x') {
                            escBufferOn = true;
                            currentState = string_lit_backslash_x_s;
                        } else {
                            flushEscSeqBuffer(buffer, escSeqBuffer);
                            bufferOn = true;
                            currentState = string_lit_s;
                        }
                        break;
                    case string_lit_backslash_x_s:
                        if (currentChar == 'a' ||
                            currentChar == 'A' ||
                            currentChar == 'b' ||
                            currentChar == 'B' ||
                            currentChar == 'c' ||
                            currentChar == 'C' ||
                            currentChar == 'd' ||
                            currentChar == 'D' ||
                            currentChar == 'e' ||
                            currentChar == 'E' ||
                            currentChar == 'f' ||
                            currentChar == 'F') {
                            escBufferOn = true;
                            currentState = string_lit_backslash_x_1_s;
                        } else {
                            flushEscSeqBuffer(buffer, escSeqBuffer);
                            bufferOn = true;
                            currentState = string_lit_s;
                        }
                        break;
                    case string_lit_backslash_x_1_s:
                        if (currentChar == 'a' ||
                            currentChar == 'A' ||
                            currentChar == 'b' ||
                            currentChar == 'B' ||
                            currentChar == 'c' ||
                            currentChar == 'C' ||
                            currentChar == 'd' ||
                            currentChar == 'D' ||
                            currentChar == 'e' ||
                            currentChar == 'E' ||
                            currentChar == 'f' ||
                            currentChar == 'F') {
                            // converting to the correct format
                            writeToBuffer(escSeqBuffer, currentChar);
                            dynStr_t *number = dstrSubstring(escSeqBuffer, 1, 4);
                            dstrPrepend(number, "0");
                            // converting from hex string to int
                            int res = (int) strtol(dstrGet(number), NULL, 0);
                            // converting back to string
                            char string[4];
                            sprintf(string, "%d", res);
                            // writing the number to the buffer
                            dstrAppend(buffer, string);
                            dstrFree(number);

                            currentState = string_lit_s;
                        } else {
                            flushEscSeqBuffer(buffer, escSeqBuffer);
                            bufferOn = true;
                            currentState = string_lit_s;
                        }
                        break;
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        break;
                    case colon_f_s:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
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
                switch (currentState) {
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
                    case string_lit_backslash_x_s:
                        escBufferOn = true;
                        currentState = string_lit_backslash_x_1_s;
                        break;
                    case string_lit_backslash_x_1_s:
                        // converting to the correct format
                        writeToBuffer(escSeqBuffer, currentChar);
                        dynStr_t *number = dstrSubstring(escSeqBuffer, 1, 4);
                        dstrPrepend(number, "0");
                        // converting from hex string to int
                        int res = (int) strtol(dstrGet(number), NULL, 0);
                        // converting back to string
                        char string[4];
                        sprintf(string, "%d", res);
                        // writing the number to the buffer
                        dstrAppend(buffer, string);
                        dstrFree(number);

                        currentState = string_lit_s;
                        break;
                    case string_lit_backslash_s:
                        if (currentChar >= 48 && currentChar <= 55) {
                            escBufferOn = true;
                            currentState = string_lit_backslash_1_s;
                        } else {
                            flushEscSeqBuffer(buffer, escSeqBuffer);
                            bufferOn = true;
                            currentState = string_lit_s;
                        }
                        break;
                    case string_lit_backslash_1_s:
                        if (currentChar >= 48 && currentChar <= 55) {
                            escBufferOn = true;
                            currentState = string_lit_backslash_2_s;
                        } else {
                            flushEscSeqBuffer(buffer, escSeqBuffer);
                            bufferOn = true;
                            currentState = string_lit_s;
                        }
                        break;
                    case string_lit_backslash_2_s:
                        if (currentChar >= 48 && currentChar <= 55) {
                            // converting to the correct format
                            writeToBuffer(escSeqBuffer, currentChar);
                            dynStr_t *number = dstrSubstring(escSeqBuffer, 1, 4);
                            // converting from octal string to int
                            char *octal = dstrGet(number);
                            char octalArray[3];
                            for (int i = 0; i < 3; i++) {
                                octalArray[i] = octal[i];
                            }
                            int res = (octalArray[0] - 48) * (8 * 8) + (octalArray[1] - 48) * (8) + (octalArray[2] - 48);
                            // checking if the number is in range
                            if (res < 1 || res > 255) {
                                flushEscSeqBuffer(buffer, escSeqBuffer);
                                currentState = string_lit_s;
                            } else {
                                // converting back to string
                                char string[4];
                                sprintf(string, "%d", res);
                                // writing the number to the buffer
                                dstrAppend(buffer, string);
                                currentState = string_lit_s;
                            }
                            // freeing the support buffer
                            dstrFree(number);
                        } else {
                            flushEscSeqBuffer(buffer, escSeqBuffer);
                            bufferOn = true;
                            currentState = string_lit_s;
                        }
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            // TODO special characters
            case '=':
                switch (currentState)
                {
                    case init_s:
                        currentState = equals_f_s;
                        break;
                    case equals_f_s:
                        currentState = eq_2_s;
                        break;
                    case eq_2_s:
                        currentState = eq_f_s;
                        stop = true;
                        break;
                    case lesser_than_f_s:
                        currentState = lesser_eq_f_s;
                        stop = true;
                        break;
                    case greater_than_f_s:
                        currentState = greater_eq_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case ':':
                switch (currentState) {
                    case init_s:
                        currentState = colon_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case ';':
                switch (currentState) {
                    case init_s:
                        currentState = semicolon_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '(':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case ')':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '{':
                switch (currentState) {
                    case init_s:
                        currentState = left_curly_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '}':
                switch (currentState) {
                    case init_s:
                        currentState = right_curly_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '$':
                switch (currentState) {
                    case init_s:
                        currentState = identifier_var_dollar_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    case string_lit_backslash_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '_':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '?':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '>':
                switch (currentState)
                {
                    case init_s:
                        currentState = greater_than_f_s;
                        break;
                    case null_f_s:
                        endingMark = true;
                        currentState = init_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '\"':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '.':
                switch (currentState) {
                    case init_s:
                        currentState = concatenation_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case ',':
                switch (currentState) {
                    case init_s:
                        currentState = comma_f_s;
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '+':
            case '-':
                switch (currentState) {
                    case init_s:
                        if (currentChar == '+')
                        {
                            currentState = plus_f_s;
                        }
                        else
                        {
                            currentState = minus_f_s;
                        }
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '/':
                switch (currentState) {
                    case init_s:
                        currentState = division_f_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case division_f_s:
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '*':
                switch (currentState) {
                    case init_s:
                        currentState = multiplication_f_s;
                        stop = true;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case division_f_s:
                        currentState = com_block_s;
                        break;
                    case com_block_s:
                        currentState = com_block_ast_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            case '<':
                switch (currentState)
                {
                    case init_s:
                        currentState = lesser_than_f_s;
                        break;
                    case string_lit_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case com_block_s:
                        currentState = com_block_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            // escape sequences
            case '\\':
                switch (currentState) {
                    case string_lit_s:
                        clearBuffer(escSeqBuffer);
                        escBufferOn = true;
                        currentState = string_lit_backslash_s;
                        break;
                    case com_line_f_s:
                        currentState = com_line_f_s;
                        break;
                    case com_block_s:
                    case com_block_ast_s:
                        currentState = com_block_s;
                        break;
                    case string_lit_backslash_s:
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
            // TODO whitespaces
            case ' ':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        stop = true;
                        break;
                }
                break;
            case '\n':
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
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
                switch (currentState) {
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
                    case string_lit_backslash_s:
                    case string_lit_backslash_1_s:
                    case string_lit_backslash_2_s:
                    case string_lit_backslash_x_s:
                    case string_lit_backslash_x_1_s:
                        flushEscSeqBuffer(buffer, escSeqBuffer);
                        bufferOn = true;
                        currentState = string_lit_s;
                        break;
                    default:
                        ungetNextChar(stream, currentChar);
                        stop = true;
                        break;
                }
                break;
        }
        // if current state changed from init_s to something else, sets token position
        if (isInit) {
            if (currentState != init_s) {
                // sets the position
                outputToken.rowNumber = row;
                outputToken.rowPosNumber = rowPos;
            }
        }

        // writes to the buffer if the buffer switch is on
        if (bufferOn) {
            writeToBuffer(buffer, currentChar);
        }
        if (escBufferOn) {
            writeToBuffer(escSeqBuffer, currentChar);
        }
    }

    // TODO assigns correct lexType to the output token
    if (!endingMark) {
        switch (currentState) {
            // literal states
            case integer_lit_f_s:
                outputToken.data.valueInteger = atoi(dstrGet(buffer));
                outputToken.type = integerLiteral;
                break;
            case float_lit_f_s:
            case float_lit_exp_f_s:
                outputToken.data.valueFloat = strtod(dstrGet(buffer), NULL);
                outputToken.type = floatLiteral;
                break;
            case string_lit_f_s:
                outputToken.data.valueString = dstrInitChar(dstrGet(buffer));
                outputToken.type = stringLiteral;
                break;
            // identifier states
            case identifier_func_f_s:
                outputToken.data.valueString = dstrInitChar(dstrGet(buffer));
                if (strcmp(dstrGet(buffer), "else") == 0) {
                    outputToken.type = elseKey;
                } else if (strcmp(dstrGet(buffer), "function") == 0) {
                    outputToken.type = functionKey;
                } else if (strcmp(dstrGet(buffer), "if") == 0) {
                    outputToken.type = ifKey;
                } else if (strcmp(dstrGet(buffer), "null") == 0) {
                    outputToken.type = nullKey;
                } else if (strcmp(dstrGet(buffer), "return") == 0) {
                    outputToken.type = returnKey;
                } else if (strcmp(dstrGet(buffer), "void") == 0) {
                    outputToken.type = voidKey;
                } else if (strcmp(dstrGet(buffer), "while") == 0) {
                    outputToken.type = whileKey;
                } else if (strcmp(dstrGet(buffer), "string") == 0) {
                    outputToken.type = stringKey;
                } else if (strcmp(dstrGet(buffer), "float") == 0) {
                    outputToken.type = floatKey;
                } else if (strcmp(dstrGet(buffer), "int") == 0) {
                    outputToken.type = intKey;
                }
                // special case - header check for declare
                else if (strcmp(dstrGet(buffer), "declare") == 0) {
                    outputToken.type = declareHeader;
                } else {
                    outputToken.type = identifierFunc;
                }
                break;
            case identifier_var_f_s:
                outputToken.data.valueString = dstrInitChar(dstrGet(buffer));
                outputToken.type = identifierVar;
                break;
            // null type state
            case null_f_s:
                outputToken.data.valueString = dstrInitChar(dstrGet(buffer));
                if (strcmp(dstrGet(buffer), "string") == 0) {
                    outputToken.type = stringNullKey;
                } else if (strcmp(dstrGet(buffer), "float") == 0) {
                    outputToken.type = floatNullKey;
                } else if (strcmp(dstrGet(buffer), "int") == 0) {
                    outputToken.type = intNullKey;
                } else {
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
            // curly braces states
            case right_curly_f_s:
                outputToken.type = curlyBraceRight;
                break;
            case left_curly_f_s:
                outputToken.type = curlyBraceLeft;
                break;
            // assignment states
            case equals_f_s:
                outputToken.type = equals;
                break;
            case colon_f_s:
                outputToken.type = colon;
                break;
            case semicolon_f_s:
                outputToken.type = semicolon;
                break;
            case comma_f_s:
                outputToken.type = comma;
                break;
            // operator states
            case multiplication_f_s:
                outputToken.type = multiplicationOp;
                break;
            case division_f_s:
                outputToken.type = divisionOp;
                break;
            case plus_f_s:
                outputToken.type = plusOp;
                break;
            case minus_f_s:
                outputToken.type = minusOp;
                break;
            case concatenation_f_s:
                outputToken.type = concatenationOp;
                break;
            case lesser_than_f_s:
                outputToken.type = lesserThanOp;
                break;
            case lesser_eq_f_s:
                outputToken.type = lesserEqOp;
                break;
            case greater_than_f_s:
                outputToken.type = greaterThanOp;
                break;
            case greater_eq_f_s:
                outputToken.type = greaterEqOp;
                break;
            case eq_f_s:
                outputToken.type = eqOp;
                break;
            case not_eq_f_s:
                outputToken.type = notEqOp;
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
                PrintErrorExit("Lexical error on ln %d, col %d!\n", ERR_LEX, outputToken.rowNumber, outputToken.rowPosNumber);
                break;
        }
    }
    // after the ending mark has been set
    else {
        switch (currentState) {
            case init_s:
                outputToken.type = ending;
                break;
            default:
                PrintErrorExit("Lexical error on ln %d, col %d!\n", ERR_LEX, outputToken.rowNumber, outputToken.rowPosNumber);
                break;
        }
    }

    // frees all buffers and returns the output token
    dstrFree(buffer);
    dstrFree(escSeqBuffer);
    lastToken = outputToken;
    return outputToken;
}