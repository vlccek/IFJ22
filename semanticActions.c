//
// Created by tonda on 20/10/22.
//

#include "semanticActions.h"


i3htTable_t program;

void testingSemanticAction(semanticActionInfo info) {
    loging("Semanticka akce detekovana!!!");
}

void callSemanticAction(rule *pravidlo, semanticActionInfo info) {
    if (!pravidlo)
    {
        loging("Pravidlo je NULL");
        return;
    }
    
    if (!pravidlo->semanticAction)
    {
        loging("Pravidlo nema semantickou akci. ID: %2d, nt: %s", pravidlo->id, getNonTerminalName(pravidlo->from));
        return;
    }
    pravidlo->semanticAction(info);
}


void SA_programBody(semanticActionInfo info)
{
    loging("SA_programBody");
}
void SA_Command_identifierVar(semanticActionInfo info)
{
    loging("SA_Command_identifierVar");
}

void SA_FunctionParam(semanticActionInfo info)
{
    functionParam(program, info.lastToken);
}

void SA_FceCall(semanticActionInfo info)
{
    startFunctionCall(info.lastToken);
}

void endOfParsing(){
    generate(program);
}

void semanticActionsInit() {
    initIgen(program);
    initializeProgram(program);

    setSemanticAction(ProgramBody, ifKey, &testingSemanticAction);
    setSemanticAction(ProgramBody, identifierVar, &SA_programBody);
    setSemanticAction(Command, identifierVar, &SA_Command_identifierVar);
    setSemanticAction(Statement, stringLiteral, &SA_FunctionParam);
    setSemanticAction(FceCall, identifierFunc, &SA_FceCall);
}