/**
 * @file symtable.h
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @author Jindřich Vodák (xvodak06@fit.vutbr.cz)
 * @brief Zdrojový soubor pomocných funkcí a maker
 * Implementace překladače imperativního jazyka IFJ21
 *
 */

#include "common.h"

char *allNonTerminalNames[] = {
        
};

char *allTerminalNames[] = {
        // keywords
        "elseKey",
        "functionKey",
        "ifKey",
        "nullKey",
        "returnKey",
        "voidKey",
        "whileKey",

        // data types
        "stringKey", // "'string' $hello"
        "floatKey",
        "intKey",
        "stringNullKey",
        "floatNullKey",
        "intNullKey",

        // assignment
        "equals",
        "colon",
        "semiColon",
        "comma",

        // operators
        "multiplicationOp",
        "divisionOp",
        "plusOp",
        "minusOp",
        "concatenationOp",
        "lesserThanOp",
        "lesserEqOp",
        "greaterThanOp",
        "greaterEqOp",
        "eqOp",
        "notEqOp",

        // parentheses
        "leftPar",
        "rightPar",

        // literals
        "stringLiteral", // "hello"
        "integerLiteral",
        "decimalLiteral",

        // identifiers
        "identifierFce",
        "identifierVar",

        // unknown
        "unknown",

        // EOF type
        "ending",
        "lexTypeCount"
};

char *allPrecedentTerminalNames[] = {
};


char *getPrecedentTerminalName(int i) {
    return allPrecedentTerminalNames[i];
}
char *getTerminalName(int i) {
    return allTerminalNames[i];
}
char *getNonTerminalName(int i) {
    return allNonTerminalNames[i];
}

void printErrOnLine(int rowNum, int rowPos) {
    fprintf(stderr, "Wrong syntax on:\n");
    fprintf(stderr, "Line: %d \n", rowNum);
    fprintf(stderr, "Char: %d \n", rowPos);
}

void printArgsEnd(const char *format, va_list args) {
    vfprintf(stderr, format, args);
    va_end (args);
}

void pErrArgsSyntax(int terminalEnum, int rowNum, int rowPos, char *format, va_list args) {
    printArgsEnd(format, args);

    fprintf(stderr, "\nUnexpected token: %s\n", getTerminalName(terminalEnum));
    printErrOnLine(rowNum, rowPos);
    fprintf(stderr, "ERR_CODE: %d \n", ERR_SYNTAX);
}

void pErrSyntax(int terminalEnum, int rowNum, int rowPos, char *format, ...) {
    va_list args;
    va_start (args, format);
    printArgsEnd(format, args);

    fprintf(stderr, "Unexpected token: %s\n", getTerminalName(terminalEnum));
    printErrOnLine(rowNum, rowPos);
    fprintf(stderr, "ERR_CODE: %d \n", ERR_SYNTAX);
}

void pErrLexer(int rowNum, int rowPos, char *format, ...) {
    va_list args;
    va_start (args, format);
    printArgsEnd(format, args);

    printErrOnLine(rowNum, rowPos);
    fprintf(stderr, "ERR_CODE: %d \n", ERR_LEX);
}

void pErrSemantic(int errCode, char *format, ...) {
    va_list args;
    va_start (args, format);
    printArgsEnd(format, args);

    fprintf(stderr, "ERR_CODE: %d \n", errCode);
}