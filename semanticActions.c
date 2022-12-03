/**
 * @file semanticActions.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Semantické akce
 * Implementace překladače jazyka IFJ22
 */

#include "semanticActions.h"


i3Table_t program;

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

void SA_Statement(semanticActionInfo info)
{
    newStatement(program, info.lastToken);
}

void SA_FceCall(semanticActionInfo info)
{
    startFunctionCall(info.lastToken);
}

void endOfParsing(){
    generate(program);
}

void SA_DeclareNewVariable(semanticActionInfo info) {
    newVariable(program, info.lastToken);
}

void semanticActionsInit() {
    initIgen(program);
    initializeProgram(&program);

    setSemanticAction(ProgramBody, ifKey, &testingSemanticAction);
    setSemanticAction(ProgramBody, identifierVar, &SA_programBody);
    setSemanticAction(Command, identifierVar, &SA_Command_identifierVar);

    // Function calls
    setSemanticAction(FceCall, identifierFunc, &SA_FceCall);
    setSemanticAction(Statement, stringLiteral, &SA_Statement);
    setSemanticAction(Statement, integerLiteral, &SA_Statement);
    setSemanticAction(Statement, floatLiteral, &SA_Statement);
    setSemanticAction(Statement, identifierVar, &SA_Statement);

    // Variables assignment
    setSemanticAction(Command, identifierVar, &SA_DeclareNewVariable);
}
