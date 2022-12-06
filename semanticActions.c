/**
 * @file semanticActions.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Semantické akce
 * Implementace překladače jazyka IFJ22
 */

#include "semanticActions.h"


i3Table_t program;

void callSemanticAction(rule *pravidlo, semanticActionInfo info) {
    if (!pravidlo) {
        loging("Pravidlo je NULL");
        return;
    }
    if (pravidlo->semanticAction == NULL) {
        loging("Pravidlo nema semantickou akci. ID: %2d, nt: %s", pravidlo->id, getNonTerminalName(pravidlo->from));
        return;
    }
    pravidlo->semanticAction(info);
}


void SA_programBody(semanticActionInfo info) {
    loging("SA_programBody");
}
void SA_Command_identifierVar(semanticActionInfo info) {
    loging("SA_Command_identifierVar");
}

void SA_Statement(semanticActionInfo info) {
    newStatement(program, info.lastToken);
}

void SA_FceCall(semanticActionInfo info) {
    startFunctionCall(program, info.lastToken);
}
void SA_FceCallEnd(semanticActionInfo info) {
    endFunctionCall(program, info.lastToken);
}

void SA_FceDefine(semanticActionInfo info) {
    enterFunc(program, dstrGet(info.lastToken.data.valueString));
    functionDefBegin(dstrGet(info.lastToken.data.valueString));
}
void SA_FceDefParam(semanticActionInfo info) {
    functionDefParam(dstrGet(info.lastToken.data.valueString), info.lastToken);
}
void SA_FceDefType(semanticActionInfo info) {
    functionDefParamRememberType(info.lastToken.type);
}
void SA_FceDefRet(semanticActionInfo info) {
    functionDefRet(info.lastToken);
}

void SA_DeclareNewVariable(semanticActionInfo info) {
    newVariable(program, info.lastToken);
}

void SA_ExpressionAction(semanticActionInfo info) {
    switch (info.action) {
        case APlus:
            actionPlus(program);
            break;
        case AMinus:
            actionSubtraction(program);
            break;
        case ADivision:
            actionDivision(program);
            break;
        case AMultiplication:
            actionMultiplication(program);
            break;
        case AConcatenation:
            actionConcat(program);
            break;
        case AGreaterThen:
            actionGTS(program);
        case ALowerThen:
            actionLTS(program);
            break;
        case AEq:
            actionEQS(program);
            break;
        case AGreaterThenEq:
            actionGTSEQ(program);
            break;
        case ALowerThenEq:
            actionLTSEQ(program);
            break;
        case ANotAnAction:
            InternalError("Expression action called at very wrong time.");
    }
}

void SA_ifKey(semanticActionInfo info) {
    ifStart();
}

void SA_Return(semanticActionInfo info) {
    prepareReturn(program);
}

void semanticActionsInit() {
    initIgen(program);
    initializeProgram(&program);

    setSemanticAction(ProgramBody, identifierVar, &SA_programBody);
    setSemanticAction(Command, identifierVar, &SA_Command_identifierVar);

    // Function calls
    setSemanticAction(FceCall, identifierFunc, &SA_FceCall);
    setSemanticAction(Statement, stringLiteral, &SA_Statement);
    setSemanticAction(Statement, integerLiteral, &SA_Statement);
    setSemanticAction(Statement, floatLiteral, &SA_Statement);
    setSemanticAction(Statement, identifierVar, &SA_Statement);
    setSemanticAction(CommaOrEpsParam, rightPar, &SA_FceCallEnd);

    // Variables assignment
    setSemanticAction(Command, identifierVar, &SA_DeclareNewVariable);

    // Function definition
    setSemanticAction(FceHeader, identifierFunc, &SA_FceDefine);
    setSemanticActionRow(FunctionDeclareParams, &SA_FceDefType, 1, rightPar);
    setSemanticAction(DeclareParam, identifierVar, &SA_FceDefParam);
    setSemanticActionRow(FuncReturnColonType, &SA_FceDefRet, 0);

    // Expression action(s)
    setSemanticActionAllRules(Exp, &SA_ExpressionAction);

    // IF
    setSemanticAction(Condition, ifKey, &SA_ifKey);

    // Return
    setSemanticAction(Return, returnKey, &SA_Return);
}

void SA_EndOfCommand() {
    flushCommand(program);
}

void SA_EndOfBlock() {
    exitCodeBlock();
}

void SA_EndOfParsing() {
    // todo: preprocessing
    generate(program);
}
