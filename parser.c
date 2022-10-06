//
// Created by tonda on 05/10/22.
//
#include "parser.h"
#include "LLtable.h"
#include "stack.h"

typedef struct parserMemory{
    genericStack* PSAStack;
    // here goes other structures
} ParserMemory;

ParserMemory* initializeMemory() {
    make_var(mem, ParserMemory*, sizeof (ParserMemory));
    mem->PSAStack = stackInit();
    return mem;
}

int parser() {
    ParserMemory* memory =  initializeMemory();


    bool success = false;
    while (success == false) {
        PSADataType type = endOfFile;
        switch (type) {
            case endOfFile:
                success = true;
                break;
            case terminal:
                break;
            case nonTerminal:
                break;
        }
    }
    return 0;
}

