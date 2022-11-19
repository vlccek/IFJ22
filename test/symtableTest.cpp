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

        TEST(symtableTest, basicInit) {
            symtable_t table;
            symtable_t *symtable = &table;
            symInit(symtable);
            char iden[] = "readi";
            symbol_t *found = symSFunction(symtable, iden);
            ASSERT_STREQ(iden, found->identifier);
        }
        TEST(symtableTest, basicSearch) {
            symtable_t table;
            symInit(&table);
            char iden[] = "readi";
            symbol_t *found = symSearch(&table, iden);
            ASSERT_EQ(found, nullptr);
        }
        TEST(symtableTest, basicInsert) {
            symtable_t table;
            symInit(&table);
            char iden[] = "aaa";
            symbol_t symbol;
            symbol.identifier = iden;
            symbol.rowNumber = 1;
            symbol.rowPosNumber = 1;
            symbol.type = floatingNullable;
            symInsert(&table, symbol);
            symbol_t *found = symSearch(&table, iden);
            ASSERT_STREQ(iden, found->identifier);
        }
        TEST(symtableTest, basicNewLocal) {
            symtable_t table;
            symInit(&table);
            char iden[] = "aaa";
            symbol_t symbol;
            symbol.identifier = iden;
            symbol.rowNumber = 1;
            symbol.rowPosNumber = 1;
            symbol.type = floatingNullable;
            symInsert(&table, symbol);
            symbol_t *found = symSearch(&table, iden);
            symNewLocal(&table);
            printSymbol(found);
            printf("%d\n", found->symtablePos);
            symNewLocal(&table);
            symNewLocal(&table);
            found = symSearch(&table, iden);
            printSymbol(found);
            printf("%d\n", found->symtablePos);
            ASSERT_STREQ(iden, found->identifier);
        }
        TEST(symtableTest, instertAllTypes) {
            symtable_t table;
            symInit(&table);
            char buffer[20];
            for (int i = 0; i < symbolTypeLenght; i++) {
                symbol_t symbol;
                sprintf(buffer, "%c", i+'a');
                char *iden = (char *) malloc(sizeof(char) * strlen(buffer));
                strcpy(iden, buffer);
                symbol.identifier = iden;
                symbol.rowNumber = i;
                symbol.rowPosNumber = i;
                symbol.type = (symbolType_t) i;
                symInsert(&table, symbol);
                symNewLocal(&table);
            }
            for (int i = 0; i < symbolTypeLenght; i++) {
                sprintf(buffer, "%c", i+'a');
                symbol_t *found = symSearch(&table, buffer);
                ASSERT_STREQ(buffer, found->identifier);
                ASSERT_EQ(i, found->rowNumber);
                ASSERT_EQ(i, found->rowPosNumber);
                ASSERT_EQ((symbolType_t) i, found->type);
            }
            printSymtable(&table);

            // test delete local symtables works
            symDelLocal(&table);

            for (int i = symbolTypeLenght -1; i > 0; i--) {
                symDelLocal(&table);
                sprintf(buffer, "%c", i+'a');
                symbol_t *found = symSearch(&table, buffer);
                printSymbol(found);
                ASSERT_FALSE(found);
            }
            symbol_t *found = symSearch(&table, "a");
            ASSERT_STREQ(found->identifier, "a");
        }

    }// namespace symtable
}// namespace ifj22
