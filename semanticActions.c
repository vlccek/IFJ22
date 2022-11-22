//
// Created by tonda on 20/10/22.
//

#include "semanticActions.h"


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
        loging("Pravidlo nema semantickou akci. ID: %d, nt: %s", pravidlo->id, getNonTerminalName(pravidlo->from));
        return;
    }
    pravidlo->semanticAction(info);
}

void semanticActionsInit() {
    setSemanticAction(ProgramBody, ifKey, &testingSemanticAction);
}