#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main

// Hide the io function since this will segfault in testing
extern "C" {
#include "../parser.h"
// DON'T TOUCH THIS!

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
                tokens = new std::vector<token_t>;
            }

            void TearDown() override {
                delete tokens;
            }

        };

        TEST_F(ParserTest, initialTest) {
            tokensForParser({ending});

            parser();
        }

        TEST_F(ParserTest, initialTest2) {
            tokensForParser({leftPar, rightPar, ending});

            ASSERT_EXIT_SYNTAX(parser());
        }

        // region operation_concat
        TEST_F(ParserTest, concatenationOp_basic) {
            tokensForParser({stringLiteral, concatenationOp, identifierVar, ending});

            ASSERT_EXIT_SYNTAX(parser());
        }

        TEST_F(ParserTest, concatenationOp_par) {
            tokensForParser(
                    {stringLiteral, concatenationOp, leftPar, identifierVar, concatenationOp, identifierVar, rightPar,
                     ending});

            ASSERT_EXIT_SYNTAX(parser());
        }


    }
}

#endif
