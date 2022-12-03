/**
 * @file symtable.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Tabulka symbolů
 * Implementace překladače jazyka IFJ22
 *
 * Při implementaci bylo použito řešení hashtabulky z 2. domacího úkolu IAL 2021/Z
 */
#include "symtable.h"

// region DTList
DTList_T *createDTL(int count, ...) {

    DTList_T *DTL = (DTList_T *) malloc(sizeof(DTList_T));
    initDTList(DTL);

    if (count == 0)
        return DTL;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        insDTList(DTL, va_arg(args, symbolDataType_t), NULL);
    }
    va_end(args);

    return DTL;
}

/**
 * @brief vkládá prvek na konec seznamu
 * 
 * @param list DTL seznam
 * @param typ typ symbolu
 */
void insDTList(DTList_T *list, symbolDataType_t typ, char* identifier) {
    DTListMem_T *actualMem = list->first;
        DTListMem_T *newMember = malloc(sizeof(DTListMem_T));
        checkNullPointer(newMember);
        newMember->type = typ;
        newMember->next = NULL;
        newMember->identifier = identifier;
    if (list->first != NULL) {
        while (actualMem->next != NULL)
            actualMem = actualMem->next;
        actualMem->next = newMember;
    } else {
        list->first = newMember;
    }
    list->len++;
}
/**
 * @brief inicializuje nový seznam
 * 
 * @param list odkaz na nový seznam
 */
void initDTList(DTList_T *list) {
    list->first = NULL;
    list->len = 0;
}
// endregion
// region HashTable
// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
size_t ht_get_hash(const char *key) {
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (uint64_t) (unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return (size_t) (hash & (uint64_t) (MAX_HTSIZE - 1));
}

void htInit(htTable_t *table) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        (*table)[i] = NULL;
    }
}

htItem_t *htSearch(htTable_t *table, const char *key) {
    htItem_t *item = (*table)[ht_get_hash(key)];
    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

void htInsertItem(htTable_t *table, const char *key, symbol_t value) {
    htItem_t *item = htSearch(table, key);
    if (item != NULL) {
        printHashtable(table, "Original");
        item->value = value;
        loging("Symbol %s already exists, overwriting", key);
        printHashtable(table, "Overwritten");
        return;
    }
    int hash = ht_get_hash(key);
    item = malloc(sizeof(struct htItem));
    if (item == NULL) {
        fprintf(stderr, "Malloc fail in htInsertItem, key '%s'.\n", key);
        return;
    }
    item->value = value;
    item->key = key;
    item->next = (*table)[hash];
    (*table)[hash] = item;
}

symbol_t *htGetItem(htTable_t *table, const char *key) {
    htItem_t *item = htSearch(table, key);
    if (item == NULL) {
        return NULL;
    }
    return &item->value;
}

void htDeleteItem(htTable_t *table, const char *key) {
    htItem_t *item = (*table)[ht_get_hash(key)];
    htItem_t *prevItem = NULL;
    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            if (prevItem != NULL) {
                prevItem->next = item->next;
            }
            free(item);
            return;
        }
        prevItem = item;
        item = item->next;
    }
    return;
}

void htDestroy(htTable_t *table) {
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        htItem_t *item = (*table)[i];
        htItem_t *prevItem = NULL;
        while (item != NULL) {
            prevItem = item;
            item = item->next;
            free(prevItem);
        }
        (*table)[i] = NULL;
    }
}
// endregion
// region SymTable

symbol_t createSymbolVarLit(const char *name, symbolType_t type, symbolDataType_t dataType, token_t token) {
    symbol_t newSymbol;
    newSymbol.identifier = name;
    newSymbol.type = type;
    newSymbol.dataType = dataType;

    newSymbol.token = token;
    newSymbol.firstParam = NULL;
    newSymbol.returnType = undefinedDataType;

    return newSymbol;
}


symbol_t *createSymbolFunction(const char *name, symbolType_t type, DTList_T *paramList, symbolDataType_t returnType) {
    symbol_t *newSymbol = (symbol_t *) malloc(sizeof(symbol_t));
    if (newSymbol == NULL) {
        InternalError("Malloc failed.");
    }
    newSymbol->type = type;
    newSymbol->dataType = undefinedDataType;
    newSymbol->identifier = name;
    newSymbol->returnType = returnType;
    DTList_T *newPList = createDTL(0);
    if (paramList != NULL) {
        DTListMem_T *tmp = paramList->first;
        while (tmp != NULL) {
            insDTList(newPList, tmp->type, NULL);
            tmp = tmp->next;
        }
    }
    newSymbol->firstParam = newPList;
    return newSymbol;
}


void saveBuildInFunctions(symtable_t *symtable) {
    symInsertFunction(symtable, *createSymbolFunction("reads", function,
                                                      createDTL(0),
                                                      stringNullable));
    symInsertFunction(symtable, *createSymbolFunction("readi", function,
                                                      createDTL(0),
                                                      integerNullable));
    symInsertFunction(symtable, *createSymbolFunction("readf", function,
                                                      createDTL(0),
                                                      floatingNullable));
    symInsertFunction(symtable, *createSymbolFunction("write", function,
                                                      createDTL(0),
                                                      undefinedDataType));
    symInsertFunction(symtable, *createSymbolFunction("intval", function,
                                                      createDTL(1, floatingNullable),
                                                      integer));
    symInsertFunction(symtable, *createSymbolFunction("floatval", function,
                                                      createDTL(1, integerNullable),
                                                      floating));
    symInsertFunction(symtable, *createSymbolFunction("strval", function,
                                                      createDTL(1, stringNullable),
                                                      string));
    symInsertFunction(symtable, *createSymbolFunction("substring", function,
                                                      createDTL(3, string, integer, integer),
                                                      stringNullable));
    symInsertFunction(symtable, *createSymbolFunction("strlen", function,
                                                      createDTL(1, string),
                                                      integer));
    symInsertFunction(symtable, *createSymbolFunction("ord", function,
                                                      createDTL(1, string),
                                                      integer));
    symInsertFunction(symtable, *createSymbolFunction("chr", function,
                                                      createDTL(1, integer),
                                                      string));
}

void symInit(symtable_t *symtable) {
    symtable->current = symtable->main;
    symtable->isInFunction = false;
    htInit(&(symtable->functions));
    for (int i = 0; i < MAX_SYMTABLES; i++) {
        htInit(&(symtable->main[i]));
        htInit(&(symtable->infunc[i]));
    }
    symtable->last = 0;
    symtable->lastMain = 0;
    saveBuildInFunctions(symtable);
}

void symDestroy(symtable_t *symtable) {
    htDestroy(&(symtable->functions));
    for (int i = 0; i < MAX_SYMTABLES; i++) {
        htDestroy(&(symtable->main[i]));
        htDestroy(&(symtable->infunc[i]));
    }
    symtable->last = -1;
}

void symNewLocal(symtable_t *symtable) {
    if (symtable->last == MAX_SYMTABLES - 2) {
        fprintf(stderr, "ERROR: max number of local symtables exeeded.\n");
        return;
    }
    symtable->last++;
}

void symDelLocal(symtable_t *symtable) {
    if (symtable->last == 0) {
        if (symtable->isInFunction) {
            loging("INFO: Switching back to main symtable.");
            symSwitchBack(symtable);
            return;
        }
        fprintf(stderr, "ERROR: attempted to delete non existent symtable.\n");
        return;
    }
    htDestroy(&(symtable->current[symtable->last]));
    symtable->last--;
}

void symInsert(symtable_t *symtable, symbol_t symbol) {
    symbol.symtablePos = -1;
    htInsertItem(&(symtable->current[symtable->last]), symbol.identifier, symbol);
}

void symInsertFunction(symtable_t *symtable, symbol_t symbol) {
    htInsertItem(&(symtable->functions), symbol.identifier, symbol);
}

symbol_t *symSearchVar(symtable_t *symtable, const char *identifier) {
    htItem_t *found;
    for (int i = 0; i <= symtable->last; i++) {
        found = htSearch(&(symtable->main[i]), identifier);
        if (found) {
            found->value.symtablePos = symtable->last - i;
            return &(found->value);
        }
    }
    return NULL;
}

symbol_t *symSearchFunc(symtable_t *symtable, const char *identifier) {
    htItem_t *found;
    found = htSearch(&(symtable->functions), identifier);
    if (found) {
        return &(found->value);
    }
    return NULL;
}

void symSwitch(symtable_t *symtable) {
    if (symtable->isInFunction) {
        InternalError("Attempted to switch to infunc while in infunc.");
    }
    if (symtable->last > 0) {
        InternalError("Attempted to switch to infunc while in a local symtable.");
    }

    symtable->current = symtable->infunc;
    symtable->lastMain = symtable->last;
    symtable->last = 0;
    symtable->isInFunction = true;
    return;
}

void symSwitchBack(symtable_t *symtable) {
    if (!symtable->isInFunction) {
        InternalError("Attempted to switch to main while in main.");
    }
    symtable->current = symtable->main;
    symtable->last = symtable->lastMain;
    symtable->lastMain = 0;
    symtable->isInFunction = false;
    return;
}


void printHashtable(htTable_t *table, const char *index) {
    htItem_t *item;
    bool hashTableNotEmpty = false;
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        bool printLineEnd = false;
        item = (*table)[i];
        while (item != NULL) {
            if (!hashTableNotEmpty) {
                hashTableNotEmpty = true;
                printlog("---- HT %s ----\n", index);
            }
            printSymbol(&(item->value));
            printLineEnd = true;
            item = item->next;
        }
        if (printLineEnd) {
            printlog("%s", "\n");
        }
    }
}

void printSymbol(symbol_t *symbol) {
    if (!symbol) {
        printlog("(NULL)");
        return;
    }

    char *dataType;
    char *type;
    switch (symbol->type) {
        case function:
            type = "function";
            break;
        case variable:
            type = "variable";
            break;
        case literal:
            type = "literal";
            break;
        case undefinedType:
            type = "undefinedType";
            break;
        default:
            InternalError("Type of symbol (%d) is unknown", symbol->type);
    }
    switch (symbol->dataType) {
        case string:
            dataType = "string";
            break;
        case stringNullable:
            dataType = "?string";
            break;
        case floating:
            dataType = "floating";
            break;
        case floatingNullable:
            dataType = "?floating";
            break;
        case integer:
            dataType = "integer";
            break;
        case integerNullable:
            dataType = "?integer";
            break;
        case nil:
            dataType = "nil";
            break;
        case undefinedDataType:
            dataType = "undefined";
            break;
        default:
            InternalError("DataType of symbol (%d) is unknown", symbol->dataType);
            break;
    }
    printlog("(%s, %s, %s)", symbol->identifier, type, dataType);
}

void printSymtable(symtable_t *symtable) {
    printHashtable(&(symtable->functions), "functions");
    for (int i = 0; i < MAX_SYMTABLES; i++) {
        char number[16];
        sprintf(number, "%d", i);
        printHashtable(&(symtable->main[i]), number);
    }
}
// endregion