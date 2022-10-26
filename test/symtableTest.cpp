#include "gtest/gtest.h"

// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../symtable.h"

#include <stdio.h>
};

namespace ifj22 {
    namespace symtable {

        class symtableTest : public ::testing::Test {
            symtableTest() {}
        };

        TEST(symtableTest, basicTest) {
            symtable_t table;
            symtable_t *symtable = &table;
            symInit(symtable);
            saveBuildInFunctions(symtable);
            char iden[] = "readi";
            symbol_t * found = symSFunction(symtable, iden);
            printSymbol(found);
            printf("\n============\n");
            printSymtable(symtable);
            ASSERT_STREQ(iden, found->identifier);
        }

    }// namespace symtable
}// namespace ifj22
