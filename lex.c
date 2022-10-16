//
// Created by tonda on 05/10/22.
//

#include "lex.h"
#if TESTING == 1
token_t *testTokens = NULL;
#endif

token_t getToken(FILE *stream) {
    token_t t;
    return t;
}

void ungetToken(FILE *stream) {
    return ;
}

token_t *testTokens = NULL;
bool teston = false;

void preToken(FILE *a) {
    if (teston == 1) {
        (--testTokens);
    } else {
        ungetToken(a);
    }
}

token_t nextToken(FILE *a) {
    if (teston == 1) {
        return (token_t) *testTokens++;
    } else {
        return getToken(a);
    }
}
