#include "gtest/gtest.h"

// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../lex.h"

#include <stdio.h>
};

namespace ifj22 {
    namespace stack {

        class lexTest : public ::testing::Test {

        protected:

            FILE *fileForTest;

            void SetUp() override {
                fileForTest = fopen("test.txt", "ab+");
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

            FILE *prerpareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
            }
        };


        TEST_F(lexTest, function_declare) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prerpareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFce, leftPar, stringKey,
                                           identifierVar, rightPar, colon, stringKey, curlyBraceRight,
                                           returnKey, identifierFce, identifierVar, semiColon, curlyBraceLeft};
            for (auto i: tokens) {
                token_t t = getToken(fp);
                ASSERT_EQ(t.type, i);
            }


        }


    }// namespace stack
}// namespace ifj22
