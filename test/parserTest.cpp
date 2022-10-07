#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main

// Hide the io function since this will segfault in testing
extern "C" {
#include "../parser.h"
#include "../parser.c"
// DON'T TOUCH THIS!

#include <stdio.h>
};
#if(DEBUG)
#define ASSERT_EXIT_SYNTAX(functionCall) \
        functionCall;
#else
#define ASSERT_EXIT_SYNTAX(functionCall) \
        ASSERT_EXIT_CODE(functionCall, ERR_SYNTAX);
#endif

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

        class ParserTestSyntaxError : public ParserTest {
        };

        class ParserTestSuccess : public ParserTest {
        };

        TEST_F(ParserTestSuccess, initialTest) {
            tokensForParser({ending, ending});
            parser();
        }

        TEST_F(ParserTestSyntaxError, initialTest2) {
            tokensForParser({leftPar, rightPar, ending});

            ASSERT_EXIT_SYNTAX(parser());
        }

        // region operation_concat
        TEST_F(ParserTestSyntaxError, concatenationOp_basic) {
            tokensForParser({stringLiteral, concatenationOp, identifierVar, ending});

            ASSERT_EXIT_SYNTAX(parser());
        }

        TEST_F(ParserTestSyntaxError, concatenationOp_par) {
            tokensForParser(
                    {stringLiteral, concatenationOp, leftPar, identifierVar, concatenationOp, identifierVar, rightPar,
                     ending});

            ASSERT_EXIT_SYNTAX(parser());
        }


    }
}

#endif
