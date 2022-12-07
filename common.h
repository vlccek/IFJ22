/**
 * @file common.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @author Jindřich Vodák (xvodak06@stud.fit.vutbr.cz)
 * @brief Helper functions and marcos
 * Implementation IFJ22 compiler
 */

#ifndef IFJ22_COMMON_H
#define IFJ22_COMMON_H
// includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_LEX 1
#define ERR_SYNTAX 2
#define ERR_FUNCTION_IDENTIFIER 3
#define ERR_FUNC_PARAM_RET_TYPE_OR_QUANTITY 4
#define ERR_UNDEFINED_VAR 5
#define ERR_RETURN_QUANTITY 6
#define ERR_TYPES 7
#define ERR_OTHER 8
#define ERR_RUNTIME 99

#define debug 0
#define debugStack 0

#define InternalError(message, args...) PrintErrorExit("%15s:%-3d | in %s() | " message "\n", \
                                                       ERR_RUNTIME,                           \
                                                       __FILE__,                              \
                                                       __LINE__,                              \
                                                       __FUNCTION__, ##args)
#define PrintErrorExit(format, ERR_CODE, ...) \
    do {                                      \
        fprintf(stderr, format, __VA_ARGS__); \
        fflush(stderr);                       \
        exit(ERR_CODE);                       \
    } while (0)
#define PrettyExit(ERR_CODE)                                \git checkout master
printlog("%15s:%-3d | in %s() | Exit with code: %d \n", \
             __FILE__,                                      \
             __LINE__,                                      \
             __FUNCTION__,                                  \
             ERR_CODE);                                     \
    exit(ERR_CODE)

#define printlog(...) \
    do { fprintf(stderr, __VA_ARGS__); } while (0)
#define loging(message, args...)                                      \
    if (debug == 1) { printlog("%15s:%-3d | in %s() | " message "\n", \
                               __FILE__,                              \
                               __LINE__,                              \
                               __FUNCTION__, ##args); }


#define checkNullPointer(p)                   \
    if ((p) == NULL) {                        \
        InternalError("Not enough memory. "); \
        exit(ERR_RUNTIME);                    \
    }// for malloc
#define checkNotZeroReturn(Expression) \
    {                                  \
        int err;                       \
        if ((err = Expression) != 0) { \
            return err;                \
        }                              \
    }

/**
* Generates code for malloc (if null and err output)
*/
#define make_var(name, type, size)                                                               \
    type name;                                                                                   \
    if ((name = (type) malloc(size)) == NULL) {                                                  \
        fprintf(stderr,                                                                          \
                "Not enought memory (malloc err) in line `%d`, in file `%s`, in function: `%s`", \
                __LINE__, __FILE__, __func__);                                                   \
        exit(ERR_RUNTIME);                                                                       \
    };


void pErrArgsSyntax(int terminalEnum, int rowNum, int rowPos, char *format, va_list args);

void pErrSyntaxExit(int terminalEnum, int rowNum, int rowPos, const char *format, ...);

void pErrLexer(int rowNum, int rowPos, char *format, ...);

void pErrSemantic(int errCode, char *format, ...);

void pErrDivideZero(int rowNum, int rowPos, char *format, ...);

char *getTerminalName(int i);

char *getNonTerminalName(int i);
char *getPrecedentTerminalName(int i);

#endif//IFJ22_COMMON_H
