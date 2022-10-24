/**
 * @file symtable.h
 * @author Jan Brudný (xbrudn02@fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @brief Parser jazyka
 * Implementace překladače imperativního jazyka IFJ21
 *
 */
#ifndef IFJ22_SYMTABLE_H
#define IFJ22_SYMTABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lex.h"
#include "common.h"

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
    number,
    integer,
    nil,
    undefined

} symbolType_t;

typedef struct symbol {
    char *identifier;
    symbolType_t type;
    int rowNumber;
    int rowPosNumber;
    int symtablePos;
    struct DTList *firstParam;
    symbolType_t returnType;

} symbol_t;

typedef struct htItem {
    char *key;
    symbol_t value;
    struct htItem *next;
} htItem_t;

typedef htItem_t *htTable_t[MAX_HTSIZE];

typedef struct symtable {
    htTable_t functions;
    htTable_t table[MAX_SYMTABLES];
    htTable_t second[MAX_SYMTABLES];
    int last;
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
void printHashtable(htTable_t *table, char *index);
void printSymbol(symbol_t *symbol);

size_t ht_get_hash(const char *key);
void htInit(htTable_t *table);
htItem_t *htSearch(htTable_t *table, char *key);
void htInsertItem(htTable_t *table, char *key, symbol_t data);
symbol_t *htGetItem(htTable_t *table, char *key);
void htDeleteItem(htTable_t *table, char *key);
void htDestroy(htTable_t *table);

void symInit(symtable_t *symtable);
void symDestroy(symtable_t *symtable);
void symNewLocal(symtable_t *symtable);
void symDelLocal(symtable_t *symtable);
void symInsert(symtable_t *symtable, symbol_t symbol);
void symIFunction(symtable_t *symtable, symbol_t symbol);
symbol_t *symSearch(symtable_t *symtable, char *identifier);

void initSStack(symStack_T *stack);
void pushSStack(symStack_T *stack, symbol_t *member);
symbol_t *popSStack(symStack_T *stack);

DTList_T *createDTL(int count, ...);
symbol_t *createSymbol(char *name, symbolType_t type, DTList_T *paramList, symbolType_t returnType);
void initDTList(DTList_T *list);
void insDTList(DTList_T *list, enum symbolType typ);
#endif //IFJ22_SYMTABLE_H
