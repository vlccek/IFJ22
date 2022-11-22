//
// Created by tonda on 20/10/22.
//

#ifndef STACK_SEMANTICACTIONS_H
#define STACK_SEMANTICACTIONS_H

#include "LLtable.h"
#include "semanticActionInfo.h"

// semantic actions

void semanticActionsInit();
void testingSemanticAction(semanticActionInfo info);
void callSemanticAction(rule *pravidlo, semanticActionInfo info);




#endif //STACK_SEMANTICACTIONS_H
