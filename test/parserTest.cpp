#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../parser.h"
#include "../parser.c" // DON'T TOUCH THIS!

#include <stdio.h>
};


#define ASSERT_EXIT_SYNTAX(functionCall) \
        ASSERT_EXIT_CODE(functionCall, ERR_SYNTAX);

#define ASSERT_EXIT_CODE(functionCall, number) \
        ASSERT_EXIT(functionCall, ::testing::ExitedWithCode(number), ".*");


namespace ifj22 {
    namespace parsertest {
        class ParserTest : public ::testing::Test {
        protected:

            std::vector<token> *tokens;

            void getAllTokens() {
                return;
            }

            /**bypass getToken function,
             *
             * lyxtype tokens for parse function
             */
            void tokensForParse(const std::vector<lexType> &lexTypes) {
                for (auto lexType: lexTypes) {
                    token newToken = {lexType, {}, 0, 0};
                    tokens->push_back(newToken);
                }
                // first is skipped :(
                tokens->insert(tokens->begin(), {lexTypeCount, {}, 0, 0});
                testTokens = tokens->data();
            }

            void SetUp() override {
                tokens = new std::vector<token>;
            }

            void TearDown() override {
                delete tokens;
            }

        };

        TEST_F(ParserTest, initialTest) {
            tokensForParse({ending});

            parser();
        }

        TEST_F(ParserTest, initialTest2) {
            tokensForParse({leftPar, rightPar, ending});

            ASSERT_EXIT_SYNTAX(parser());
        }


    }
}

#endif
