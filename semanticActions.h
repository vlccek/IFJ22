/**
 * @file semanticActions.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Semantic action definitions
 * Implementation IFJ22 compiler
 */

#ifndef STACK_SEMANTICACTIONS_H
#define STACK_SEMANTICACTIONS_H

#include "LLtable.h"
#include "semanticActionInfo.h"
#include "generator_igen.h"
#include "generator_generator.h"

// semantic actions

void semanticActionsInit();
void testingSemanticAction(semanticActionInfo info);
void callSemanticAction(rule *pravidlo, semanticActionInfo info);
void SA_programBody(semanticActionInfo info);
void SA_Command_identifierVar(semanticActionInfo info);
void SA_EndOfParsing();
void SA_EndOfCommand();
void SA_EndOfBlock();




#endif //STACK_SEMANTICACTIONS_H
