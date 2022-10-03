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

#ifndef LUAINTERPRET_COMMON_H
#define LUAINTERPRET_COMMON_H
// includy
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// exit cody
#define ERR_LEX 1 // 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
#define ERR_SYNTAX  2 // 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
#define ERR_IDENTIFIER_NAME 3 // 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp.
#define ERR_TYPES_ASS_COMPATIBILITY 4// 4 - sémantická chyba v příkazu přiřazení (typová nekompatibilita).
#define ERR_FUNCTION_RET_OR_PARAM 5  // 5 - sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot
#define ERR_TYPES 6  //6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních
#define ERR_SEM_OTHER 7 // • 7 - ostatní sémantické chyby.
#define ERR_NILL 8 // 8 - běhová chyba při práci s neočekávanou hodnotou nil.
#define ERR_ZERO_DIV 9 //9 běhová chyba celočíselného dělení nulovou konstantou.
#define ERR_RUNTIME 99 // interní chyba přek

// řídící makra
#define debug 1 // pokud je `1` vypisují se logy z.

#ifndef TestParser
#define TestParser 0 // pro testování parseru nastavuje chování makra printErr
#endif

#ifndef mainFromExpParser
#define mainFromExpParser 0
#endif

#define printlog(format, ...)    do{  fprintf(stderr, format, __VA_ARGS__);}while(0)
#define loging(message, args...)    if (debug == 1) {printlog("%15s:%d | in %s() | " message "\n", __FILE__, __LINE__,  __FUNCTION__, ## args);}


// makra pro logování a easy exity
//region logginAndExitingMacros

#define InternalError(message, args...)     PrintErrorExit("%15s:%d | in %s() | " message "\n", __FILE__, __LINE__,  __FUNCTION__, ## args)
#define PrintErrorExit(format, ...)    do{  if (!debug) {fprintf(stderr, format, __VA_ARGS__);}; fflush(stderr); exit(ERR_RUNTIME);}while(0)

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


void pErrArgsSyntax(int terminalEnum, int rowNum, int rowPos, char *format, va_list args);
void pErrSyntax(int terminalEnum, int rowNum, int rowPos, char *format, ...);
void pErrLexer(int rowNum, int rowPos, char *format, ...);
void pErrSemantic(int errCode, char *format, ...);
void pErrDivideZero(int rowNum, int rowPos, char *format, ...);
char *getTerminalName(int i);
char *getNonTerminalName(int i);
char *getPrecedentTerminalName(int i);

#endif //LUAINTERPRET_COMMON_H