//
// Created by tonda on 05/10/22.
//
#include "parser.h"



void initialize() {

}

int parser() {
    initialize();

    token t;
    while(true){
        t = nextToken(stdin);
        if (t.type == ending)
            break;

        //printf( "%s", getTerminalName((int) t.type));
    }
    return 0;

    bool success = false;
    while (success == false) {
        typeOnTop type = endOfFile;
        switch (type) {
            case endOfFile:
                success = true;
                break;
            case terminal:
                break;
            case nonTerminal:

                break;
            case fail:
                InternalError("Error - Precedence op in TopDownParser stack.");
        }
    }
    return 0;
}

