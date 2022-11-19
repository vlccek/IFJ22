#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main

// Hide the io function since this will segfault in testing
extern "C" {
#include "../expParse.h"
#include "../lex.h"
// DON'T TOUCH THIS!
#include <stdio.h>
}


#define ASSERT_EXIT_SYNTAX(functionCall) \
    ASSERT_EXIT_CODE(functionCall, ERR_SYNTAX);

#define ASSERT_EXIT_CODE(functionCall, number) \
    ASSERT_EXIT(functionCall, ::testing::ExitedWithCode(number), ".*");


namespace ifj22 {
    namespace parsertest {
        class expParserTest : public ::testing::Test {
        protected:
            std::vector<token_t> *tokens;

            void getAllTokens() {
                return;
            }

            /**bypass getToken function,
             *
             * lyxtype tokens for parse function
             */
            void tokensForParser(const std::vector<lexType> &lexTypes) {
                for (auto lexType: lexTypes) {
                    token_t newToken = {lexType, {}, 0, 0};
                    tokens->push_back(newToken);
                }

                testTokens = tokens->data();
            }

            void SetUp() override {
                teston = true;
                tokens = new std::vector<token_t>;
            }

            void TearDown() override {
                delete tokens;
            }
        };

        TEST_F(expParserTest, basicTestExp) {
            tokensForParser({integerLiteral, plusOp, integerLiteral, semicolon});

            expAnal();
        }
        TEST_F(expParserTest, basicTestExp2) {
            tokensForParser({integerLiteral, minusOp, integerLiteral, semicolon});

            expAnal();
        }
        TEST_F(expParserTest, basicTestExp5) {
            tokensForParser({integerLiteral, multiplicationOp, integerLiteral, semicolon});

            expAnal();
        }
        TEST_F(expParserTest, basicTestExp4) {
            tokensForParser({integerLiteral, divisionOp, integerLiteral, semicolon});

            expAnal();
        }
        TEST_F(expParserTest, basicTestExp3) {
            tokensForParser({integerLiteral, concatenationOp, integerLiteral, semicolon});

            expAnal();
        }

    }// namespace parsertest
}// namespace ifj22

#endif
