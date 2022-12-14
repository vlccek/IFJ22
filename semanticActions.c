/**
 * @file semanticActions.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Semantic action definitions
 * Implementation IFJ22 compiler
 */

#include "semanticActions.h"


i3Table_t program;

void callSemanticAction(rule *pravidlo, semanticActionInfo info) {
    if (!pravidlo) {
        loging("Pravidlo je NULL");
        return;
    }
    if (pravidlo->semanticAction == NULL) {
        loging("Pravidlo nema semantickou akci. nt: %s, t: %s",
               getNonTerminalName(pravidlo->from),
               getTerminalName(info.lastToken.type));
        return;
    }
    pravidlo->semanticAction(info);
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
            break;
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
        case ANotEq:
            actionNEQS(program);
            break;
        case ANotAnAction:
            InternalError("Expression action called at very wrong time.");
    }
}

void SA_Return(semanticActionInfo info) {
    prepareReturn(program);
}

void SA_NewSymtableFrame(semanticActionInfo info) {
    createNewSymtableFrame();
}

void SA_ifKey(semanticActionInfo info) {
    SA_NewSymtableFrame(info);
    ifStart();
}

void SA_elseKey(semanticActionInfo info) {
    SA_NewSymtableFrame(info);
    elseStart();
}

void SA_Destroy_NULL(semanticActionInfo info) {
    printlog("NULL nelze přiřadit přímo do proměnné!");
    PrettyExit(ERR_TYPES);
}

void SA_NewCommand() {
    checkIfHaveElseBranch(program);
}

void SA_whileKey(semanticActionInfo info) {
    SA_NewSymtableFrame(info);
    whilestarts(program);
}

void semanticActionsInit() {
    initIgen(program);
    initializeProgram(&program);

    // Function calls
    setSemanticAction(FceCall, identifierFunc, &SA_FceCall);
    setSemanticAction(Statement, stringLiteral, &SA_Statement);
    setSemanticAction(Statement, integerLiteral, &SA_Statement);
    setSemanticAction(Statement, floatLiteral, &SA_Statement);
    setSemanticAction(Statement, identifierVar, &SA_Statement);
    setSemanticAction(CommaOrEpsParam, rightPar, &SA_FceCallEnd);
    setSemanticAction(FirstFceParam, rightPar, &SA_FceCallEnd);

    // Variables assignment
    setSemanticAction(DeclareVariable, identifierVar, &SA_DeclareNewVariable);
    setSemanticAction(DefVarAss, nullKey, &SA_Destroy_NULL);

    // Function definition
    setSemanticAction(FceHeader, identifierFunc, &SA_FceDefine);
    setSemanticActionRow(DataType, &SA_FceDefType, 0);
    setSemanticAction(DeclareParam, identifierVar, &SA_FceDefParam);
    setSemanticActionRow(FuncReturnColonType, &SA_FceDefRet, 0);

    // Expression action(s)
    setSemanticActionAllRules(Exp, &SA_ExpressionAction);

    // IF
    setSemanticAction(Condition, ifKey, &SA_ifKey);
    setSemanticAction(ElseCond, elseKey, &SA_elseKey);
    setSemanticActionRow(Command, &SA_NewCommand, 0);

    // while
    setSemanticAction(While, whileKey, &SA_whileKey);


    // Return
    setSemanticAction(Return, returnKey, &SA_Return);

    // Creating new symtable frame
    // setSemanticAction(While, whileKey, &SA_NewSymtableFrame);
}


void SA_EndOfCommand() {
    flushCommand(program);
}

void SA_EndOfBlock() {
    exitCodeBlock(program);
}

void SA_EndOfParsing() {
    finalGeneration(program);
}
