/**
 * @file symtable.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Tabulka symbolů
 * Implementace překladače jazyka IFJ22
 *
 * Při implementaci bylo použito řešení hashtabulky z 2. domacího úkolu IAL 2021/Z
 */
#ifndef IFJ22_SYMTABLE_H
#define IFJ22_SYMTABLE_H

#include "common.h"
#include "lex.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// musí být 2^n
#define MAX_HTSIZE 128

// definuje maximální zanoření v programu (kolik může být zanořených rámců)
#define MAX_SYMTABLES 128

// konstanty pro FNV-1a
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef enum symbolType {
    function,
    string,
    stringNullable,
    floating,
    floatingNullable,
    integer,
    integerNullable,
    nil,
    undefined, 
    symbolTypeLenght

} symbolType_t;

// Todo: we have a great problem here:
// we need to know that symbol is variable, but also the type of the variable
// so we need something like data type and symbol type???
typedef struct symbol {
    const char *identifier;
    symbolType_t type;
    int symtablePos;
    struct DTList *firstParam;
    symbolType_t returnType;
    struct symbol_data{
        char* string;
        int integer;
        double floating;
    } symbolData;
    int rowNumber;
    int rowPosNumber;
} symbol_t;

typedef struct htItem {
    const char *key;
    symbol_t value;
    struct htItem *next;
} htItem_t;

typedef htItem_t *htTable_t[MAX_HTSIZE];

typedef struct symtable {
    htTable_t functions;
    htTable_t *current;
    htTable_t main[MAX_SYMTABLES];
    htTable_t infunc[MAX_SYMTABLES];
    bool isInFunction;
    int last;
    int lastMain;
} symtable_t;

typedef struct DTListMem DTListMem_T;
typedef struct DTListMem {
    DTListMem_T *next;
    symbolType_t type;
} DTListMem_T;

typedef struct DTList {
    int len;
    DTListMem_T *first;
} DTList_T;

typedef struct symStackMem symStackMem_T;
typedef struct symStackMem {
    symStackMem_T *down;
    symbol_t *s;
} symStackMem_T;

typedef struct symStack symStack_T;
typedef struct symStack {
    int len;
    symStackMem_T *top;

} symStack_T;

void printSymtable(symtable_t *symtable);
void printHashtable(htTable_t *table, const char *index);
void printSymbol(symbol_t *symbol);

size_t ht_get_hash(const char *key);
void htInit(htTable_t *table);
htItem_t *htSearch(htTable_t *table, const char *key);
void htInsertItem(htTable_t *table, const char *key, symbol_t data);
symbol_t *htGetItem(htTable_t *table, const char *key);
void htDeleteItem(htTable_t *table, const char *key);
void htDestroy(htTable_t *table);

void symInit(symtable_t *symtable);
void symDestroy(symtable_t *symtable);
void symNewLocal(symtable_t *symtable);
void symDelLocal(symtable_t *symtable);
void symInsert(symtable_t *symtable, symbol_t symbol);
void symIFunction(symtable_t *symtable, symbol_t symbol);
symbol_t *symSearchVar(symtable_t *symtable, const char *identifier);
symbol_t *symSearchFunc(symtable_t *symtable, const char *identifier);
void symSwitch(symtable_t *symtable);
void symSwitchBack(symtable_t *symtable);

DTList_T *createDTL(int count, ...);
symbol_t *createSymbol(const char *name, symbolType_t type, DTList_T *paramList, symbolType_t returnType);
void saveBuildInFunctions(symtable_t *symtable);
void initDTList(DTList_T *list);
void insDTList(DTList_T *list, enum symbolType typ);
#endif//IFJ22_SYMTABLE_H
