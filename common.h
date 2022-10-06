/**
 * @file symtable.h
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @author Jindřich Vodák (xvodak06@fit.vutbr.cz)
 * @brief Hlavičkový soubor pomocných funkcí a maker
 * Implementace překladače imperativního jazyka IFJ21
 *
 */

#ifndef IFJ22_COMMON_H
#define IFJ22_COMMON_H
// includy
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// exit codsorry mojey
#define ERR_LEX 1 // 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
#define ERR_SYNTAX  2 // 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.).
#define ERR_IDENTIFIER_NAME 3 // 3 - sémantická chyba v programu – nedefinovaná funkce, pokus o redefinice funkce
#define ERR_TYPES_ASS_COMPATIBILITY 4// 4 - špatný počet/typ parametrů u volání funkce či typ návratové hodnoty z funkce.
#define ERR_FUNCTION_RET_OR_PARAM 5  // 5 - sémantická chyba v programu – použití nedefinované proměnné.
#define ERR_TYPES 6  //6 - sémantická/běhová chyba v programu – chybějící/přebývající výraz v příkazu návratu z funkce
#define ERR_SEM_OTHER 7 // • 7 - émantická/běhová chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
#define ERR_NILL 8 // 8 - ostatní sémantické chyby.
#define ERR_RUNTIME 99 //  interní chyba překladače tj. neovlivněná vstupním programem

// řídící makra
#define debug 1 // pokud je `1` vypisují se logy z pomocí maker printErr

#define printlog(format, ...)    do{  fprintf(stderr, format, __VA_ARGS__);}while(0)
#define loging(message, args...)    if (debug == 1) {printlog("%15s:%d | in %s() | " message "\n", __FILE__, __LINE__,  __FUNCTION__, ## args);}


// makra pro logování a easy exity
//region logginAndExitingMacros

#define Error(message,ERR_CODE, args...)     PrintErrorExit("%15s:%d | in %s() | " message "\n", ERR_CODE ,__FILE__, __LINE__,  __FUNCTION__, ## args)
#define PrintErrorExit(format,ERR_CODE,   ...)    do{  if (!debug) {fprintf(stderr, format, __VA_ARGS__);}; fflush(stderr); exit(ERR_CODE);}while(0)

#define printlog(format, ...)    do{  fprintf(stderr, format, __VA_ARGS__);}while(0)
#define loging(message, args...)    if (debug == 1) {printlog("%15s:%d | in %s() | " message "\n", __FILE__, __LINE__,  __FUNCTION__, ## args);}
//endregion


// makra pro příjemější programování
#define checkNullPointer(p)   if ((p) == NULL){InternalError("Not enough memory. "); exit(99);} // pro malloc
#define checkNotZeroReturn(Expression) {\
                int err;\
                if((err = Expression) != 0){  \
                    return err; \
                }\
                }

/**
* Generates code for malloc (if null and err output)
*/
#define make_var(name, type, size) \
type name;                                                                              \
if ((name = (type )malloc(size) ) == NULL) {                                             \
    fprintf(stderr,                                                                     \
    "Not enought memory (malloc err) in line `%d`, in file `%s`, in function: `%s`",    \
    __LINE__, __FILE__, __func__);                                                      \
    exit(ERR_RUNTIME);                                                                   \
    };                                                                                  \



void pErrArgsSyntax(int terminalEnum, int rowNum, int rowPos, char *format, va_list args);

void pErrSyntax(int terminalEnum, int rowNum, int rowPos, char *format, ...);

void pErrLexer(int rowNum, int rowPos, char *format, ...);

void pErrSemantic(int errCode, char *format, ...);

void pErrDivideZero(int rowNum, int rowPos, char *format, ...);

char *getTerminalName(int i);

char *getNonTerminalName(int i);
char *getPrecedentTerminalName(int i);

#endif //LUAINTERPRET_COMMON_H
