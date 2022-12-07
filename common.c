/**
 * @file common.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Helper functions and marcos
 * Implementation IFJ22 compiler
 */

#include "common.h"

char *allNonTerminalNames[] = {
        // S - initial state
         "ProgramBody",

        // Command
         "Command",

        // Function definition
         "FceDefine",
         "FceHeader",
         "FunctionDeclareParams",
         "CommaOrEpsParams",
         "DeclareParam",

        // FuncReturnType
         "FuncReturnColonType",

        // Function call
         "FceCall",
         "FirstFceParam",
         "CommaOrEpsParam",

        // Exp
         "Exp",
         "Statement",

        // Data types DataType
         "DataType",

        // Variable definition
         "DeclareVariable",
         "DefVarAss",

        // Conditions
         "Condition",
         "ElseCond",

        // While
         "While",

        // Return
         "Return",
         "ReturnExp",

        // Function body
         "FunctionBody",
        "nonTerminalCount"
};

char *allTerminalNames[] = {
        // keywords
        // keywords
        "elseKey",
        "functionKey",
        "ifKey",
        "nullKey",
        "returnKey",
        "voidKey",
        "whileKey",
        "stringKey",
        "floatKey",
        "intKey",
        "stringNullKey",
        "floatNullKey",
        "intNullKey",

        // declare header
        "declareHeader",

        // curly braces
        "curlyBraceRight",
        "curlyBraceLeft",

        // assignment
        "equals",
        "colon",
        "semicolon",
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
        "stringLiteral",// "hello"
        "integerLiteral",
        "floatLiteral",

        // identifiers
        "identifierFunc",
        "identifierVar",

        // unknown
        "unknown",

        // EOF type
        "ending",
        "lexTypeCount"};

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

void pErrSyntaxExit(int terminalEnum, int rowNum, int rowPos, const char *format, ...) {
    va_list args;
    va_start (args, format);
    printArgsEnd(format, args);

    fprintf(stderr, "Unexpected token: %s\n", getTerminalName(terminalEnum));
    printErrOnLine(rowNum, rowPos);
    fprintf(stderr, "ERR_CODE: %d \n", ERR_SYNTAX);
    exit(ERR_SYNTAX);
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