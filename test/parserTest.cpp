#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../parser.h"

#include <stdio.h>
};

#define ASSERT_EXIT_SYNTAX(functionCall) \
        ASSERT_EXIT(functionCall, ::testing::ExitedWithCode(ERR_SYNTAX), ".*");

#define ASSERT_EXIT_CODE(functionCall, number) \
        ASSERT_EXIT(functionCall, ::testing::ExitedWithCode(number), ".*");

namespace ifj22 {
    namespace parsertest {
        class ParserTest : public ::testing::Test {
        protected:

            FILE *fileForTest;

            void SetUp() override {
                fileForTest = fopen("test.txt", "ab+");
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                return fileForTest;
            }

            void getAllTokens() {
                return;
            }
            std::vector<token> createTokens(const std::vector<lexType>& lexTypes){
                std::vector<token> tokens =  {};
                for (auto lexType : lexTypes){
                    token newToken = {lexType, {}, 0, 0};
                    tokens.push_back(newToken);
                }
                return tokens;
            }
        };

        TEST_F(ParserTest, initialTest) {
            std::vector<token> tokens = createTokens({ ending });

            testParserTokens(tokens.data());
        }

        TEST_F(ParserTest, initialTest2) {
            std::vector<token> tokens = createTokens({ leftPar, rightPar, ending });

            ASSERT_EXIT_SYNTAX(testParserTokens(tokens.data()););
        }


    }
}

#endif