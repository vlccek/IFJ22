//
// Created by tonda on 20/10/22.
//

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
void endOfParsing();




#endif //STACK_SEMANTICACTIONS_H
