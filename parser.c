//
// Created by tonda on 05/10/22.
//
#include "parser.h"

token (*nextToken)(FILE *fp) = getToken;

token *testTokens;

token getTokenTesting(FILE *fp) {
    return *testTokens++;
}

void initialize() {

}

int parse(int argc, char *argv[]) {
    initialize();

    token t;
    while(true){
        t = nextToken(stdin);
        if (t.type == ending)
            break;

        printf( "%s", getTerminalName((int) t.type));
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

int testParserTokens(token *tokens) {
    testTokens = tokens;
    nextToken = getTokenTesting;
    parse(0, "");
}
