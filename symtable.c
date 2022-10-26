/**
 * @file symtable.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@fit.vutbr.cz)
 * @brief Tabulka symbolů
 * Implementace překladače imperativního jazyka IFJ21
 *
 * Při implementaci bylo použito řešení hashtabulky z 2. domacího úkolu IAL
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
        insDTList(DTL, va_arg(args, symbolType_t));
    }
    va_end(args);

    return DTL;
}

/*
 * Insertuje prvky seznamu
 * PRÁZDNÝ SEZNAM MUSÍ BÝT NULL!
 */
void insDTList(DTList_T *list, enum symbolType typ) {
    DTListMem_T *actualMem = list->first;
    if (list->first != NULL) {
        while (actualMem->next != NULL)
            actualMem = actualMem->next;
        DTListMem_T *newMember = malloc(sizeof(DTListMem_T));
        checkNullPointer(newMember);
        newMember->next = NULL;
        newMember->type = typ;
        actualMem->next = newMember;
    } else {
        DTListMem_T *newMember = malloc(sizeof(DTListMem_T));
        checkNullPointer(newMember);
        newMember->next = NULL;
        newMember->type = typ;
        list->first = newMember;
    }
    list->len++;
}

void initDTList(DTList_T *list) {
    list->first = NULL;
    list->len = 0;
}
// endregion
// region SStack
void initSStack(symStack_T *stack) {
    stack->len = 0;
    stack->top = NULL;
}
void pushSStack(symStack_T *stack, symbol_t *member) {
    symStackMem_T *newMember = malloc(sizeof(symStackMem_T));
    checkNullPointer(newMember);
    symStackMem_T *oldTop = stack->top;
    newMember->down = oldTop;
    newMember->s = member;
    stack->top = newMember;
    stack->len++;
}

symbol_t *popSStack(symStack_T *stack) {
    if (stack->top == NULL) {
        loging("Pop from empty stack!");
        return NULL;
    }
    symStackMem_T *returnMember = stack->top;
    stack->top = stack->top->down;
    stack->len--;
    return returnMember->s;
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

htItem_t *htSearch(htTable_t *table, char *key) {
    htItem_t *item = (*table)[ht_get_hash(key)];
    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

void htInsertItem(htTable_t *table, char *key, symbol_t value) {
    htItem_t *item = htSearch(table, key);
    if (item != NULL) {
        item->value = value;
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

symbol_t *htGetItem(htTable_t *table, char *key) {
    htItem_t *item = htSearch(table, key);
    if (item == NULL) {
        return NULL;
    }
    return &item->value;
}

void htDeleteItem(htTable_t *table, char *key) {
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

symbol_t *createSymbol(char *name, symbolType_t type, DTList_T *paramList, symbolType_t returnType) {
    symbol_t *newSymbol = (symbol_t *) malloc(sizeof(symbol_t));
    if (newSymbol == NULL) {
        InternalError("Malloc failed.");
    }
    newSymbol->type = type;
    newSymbol->identifier = name;
    newSymbol->returnType = returnType;
    DTList_T *newPList = createDTL(0);
    if (paramList != NULL) {
        DTListMem_T *tmp = paramList->first;
        while (tmp != NULL) {
            insDTList(newPList, tmp->type);
            tmp = tmp->next;
        }
    }
    newSymbol->firstParam = newPList;
    return newSymbol;
}


void saveBuildInFunctions(symtable_t *symtable) {
    symIFunction(symtable, *createSymbol("reads", function,
                                         createDTL(0),
                                         stringNullable));
    symIFunction(symtable, *createSymbol("readi", function,
                                         createDTL(0),
                                         integerNullable));
    symIFunction(symtable, *createSymbol("readf", function,
                                         createDTL(0),
                                         floatingNullable));
    symIFunction(symtable, *createSymbol("write", function,
                                         createDTL(0),
                                         undefined));
    symIFunction(symtable, *createSymbol("intval", function,
                                         createDTL(1, floatingNullable),
                                         integer));
    symIFunction(symtable, *createSymbol("floatval", function,
                                         createDTL(1, integerNullable),
                                         floating));
    symIFunction(symtable, *createSymbol("strval", function,
                                         createDTL(1, stringNullable),
                                         string));
    symIFunction(symtable, *createSymbol("substring", function,
                                         createDTL(3, string, integer, integer),
                                         stringNullable));
    symIFunction(symtable, *createSymbol("strlen", function,
                                         createDTL(1, string),
                                         integer));
    symIFunction(symtable, *createSymbol("ord", function,
                                         createDTL(1, string),
                                         integer));
    symIFunction(symtable, *createSymbol("chr", function,
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
    symtable->last = -1;
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
    if (symtable->last == MAX_SYMTABLES - 1) {
        fprintf(stderr, "ERROR: max number of local symtables exeeded.\n");
        return;
    }
    symtable->last++;
}

void symDelLocal(symtable_t *symtable) {
    if (symtable->last == -1) {
        fprintf(stderr, "ERROR: attempted to delete non existent symtable.\n");
        return;
    }
    htDestroy(&(symtable->main[symtable->last]));
    symtable->last--;
}

void symInsert(symtable_t *symtable, symbol_t symbol) {
    symbol.symtablePos = -1;
    // hodnota v symtabulce krom globalní je celkem irelevantní,
    // protože reprezentuje jak moc daleko je od aktuálního lokálního kontextu
    if (symtable->last == -1) {
        htInsertItem(&(symtable->main[0]), symbol.identifier, symbol);
        return;
    }

    htInsertItem(&(symtable->main[symtable->last]), symbol.identifier, symbol);
}
void symIFunction(symtable_t *symtable, symbol_t symbol) {
    htInsertItem(&(symtable->functions), symbol.identifier, symbol);
}

symbol_t *symSearch(symtable_t *symtable, char *identifier) {
    htItem_t *found;
    for (int i = symtable->last; i >= 0; i--) {
        found = htSearch(&(symtable->main[i]), identifier);
        if (found) {
            found->value.symtablePos = symtable->last - i;
            return &(found->value);
        }
    }
    found = htSearch(&(symtable->main[0]), identifier);

    if (found) {
        return &(found->value);
    }
    return NULL;
}

symbol_t *symSFunction(symtable_t *symtable, char *identifier) {
    htItem_t *found;
    found = htSearch(&(symtable->functions), identifier);
    if (found) {
        return &(found->value);
    }
    return NULL;
}

void symSwitch(symtable_t *symtable){
    if (symtable->isInFunction)
    {
        InternalError("Attempted to define function in function");
    }
    symtable->current = symtable->infunc;
    symtable->isInFunction = true;
    return;
}


void printHashtable(htTable_t *table, char *index) {
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
    char *type;
    switch (symbol->type) {
        case function:
            type = "function";
            break;
        case string:
            type = "string";
            break;
        case stringNullable:
            type = "?string";
            break;
        case floating:
            type = "floating";
            break;
        case floatingNullable:
            type = "?floating";
            break;
        case integer:
            type = "integer";
            break;
        case integerNullable:
            type = "?integer";
            break;
        case nil:
            type = "nil";
            break;
        case undefined:
            type = "undefined";
            break;
        default:
            InternalError("Type of symbol (%d) is unknown", symbol->type);
            break;
    }
    printlog("(%s, %s)", symbol->identifier, type);
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