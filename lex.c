#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

void printTokenData(token input)
{
    return;
}

int main()
{
    token tokens;
    tokens.type = stringLiteral;
    Data datas;
    datas.valueString = "Hello World";
    tokens.data = datas;
    tokens.rowNumber = 1;
    tokens.rowPosNumber = 1;
    printTokenData(tokens);
    return 0;
}