//
// Created by tonda on 20/10/22.
//

#include "semanticActions.h"


void testingSemanticAction(semanticActionInfo info) {
    loging("Semanticka akce detekovana!!!");
}

void semanticActionsInit() {
    setSemanticAction(ProgramBody, ifKey, &testingSemanticAction);
}