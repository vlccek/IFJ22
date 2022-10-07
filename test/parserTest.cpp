#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main

// Hide the io function since this will segfault in testing
extern "C" {
#include "../parser.h"
#include "../parser.c"
// DON'T TOUCH THIS!
#include "../dynstring.h"

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

            int uID = 0

                    void
                    getAllTokens() {
                return;
            }

            /**bypass getToken function,
             *
             * lyxtype tokens for parse function
             */
            void tokensForParser(const std::vector<lexType> &lexTypes) {
                for (auto lexType: lexTypes) {
                    token_t newToken = {lexType, {}, 0, 0};
                    switch (lexType) {
                        case integerLiteral:
                            newToken.data.valueInteger = 10;
                            break;
                        case floatLiteral:
                            newToken.data.valueFloat = 10;
                            break;
                        case stringLiteral:
                            newToken.data.valueString = dstrInitChar("hovnokod");
                            break;
                        case identifierFunc:
                        case identifierVar:
                            newToken.data.valueString = dstrInitChar(std::string("hovnokod") + std::to_string(uID));
                            break;
                    }
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
            tokensForParser({ending});
            parser();
        }

        TEST_F(ParserTestSyntaxError, initialTest2) {
            tokensForParser({leftPar, rightPar, ending});

            ASSERT_EXIT_SYNTAX(parser());
        }

        // region basic_op
        TEST_F(ParserTestSuccess, concatenationOp_basic) {
            tokensForParser({stringLiteral, concatenationOp, identifierVar, ending});

            parser();
        }
        TEST_F(ParserTestSuccess, plusOP) {
            tokensForParser({stringLiteral, plusOp, identifierVar, ending});

            parser();
        }
        TEST_F(ParserTestSuccess, minusOP) {
            tokensForParser({stringLiteral, minusOp, identifierVar, ending});

            parser();
        }
        TEST_F(ParserTestSuccess, multipl) {
            tokensForParser({stringLiteral, multiplicationOp, identifierVar, ending});

            parser();
        }
        TEST_F(ParserTestSuccess, div) {
            tokensForParser({stringLiteral, divisionOp, identifierVar, ending});

            parser();
        }
        // endregion

        // region pars_op
        TEST_F(ParserTestSyntaxError, concatenationOp_par) {
            tokensForParser(
                    {stringLiteral, concatenationOp, leftPar, identifierVar, concatenationOp, identifierVar, rightPar,
                     ending});

            parser();
        }
        TEST_F(ParserTestSyntaxError, add_par) {
            tokensForParser(
                    {stringLiteral, plusOp, leftPar, identifierVar, plusOp, identifierVar, rightPar,
                     ending});

            parser();
        }
        TEST_F(ParserTestSyntaxError, multiOp_par) {
            tokensForParser(
                    {stringLiteral, multiplicationOp, leftPar, identifierVar, multiplicationOp, identifierVar, rightPar,
                     ending});

            parser();
        }
        TEST_F(ParserTestSyntaxError, divOP_par) {
            tokensForParser(
                    {stringLiteral, divisionOp, leftPar, identifierVar, divisionOp, identifierVar, rightPar,
                     ending});

            parser();
        }

        //end region


        TEST_F(ParserTestSuccess, functionDeclaration) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            parser();
        }

        //endregion

        TEST_F(ParserTestSuccess, functionDeclaration_returnVoid) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon,
                     curlyBraceRight});

            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnInMainBody) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight,
                     returnKey, semicolon});

            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnEverywhere) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon,
                     curlyBraceRight,
                     returnKey, semicolon});

            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnOnly) {
            tokensForParser(
                    {returnKey, semicolon});
            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnOnly_ManyTimes) {
            tokensForParser(
                    {returnKey, semicolon,
                     returnKey, semicolon,
                     returnKey, semicolon});
            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_SemicolonOnly) {
            tokensForParser(
                    {semicolon});
            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_ManySemicolons) {
            tokensForParser(
                    {semicolon, semicolon, semicolon, semicolon, semicolon,});
            parser();
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_returnKeyDuplication) {
            tokensForParser(
                    {returnKey, returnKey, semicolon});

            ASSERT_EXIT_SYNTAX(parser());
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoLeftPar) {
            tokensForParser(
                    {functionKey, identifierFunc, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoRightPar) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoColon) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoReturnType) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoFunctionKey) {
            tokensForParser(
                    {identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoidentifierFunc) {
            tokensForParser(
                    {functionKey, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NocurlyBraceLeft) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NocurlyBraceRight) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleFunctionKey) {
            tokensForParser(
                    {functionKey, functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleIdentifierFunc) {
            tokensForParser(
                    {functionKey, identifierFunc, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleLeftPar) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleRightPar) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleReturnType) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleCurlyBraceLeft) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleCurlyBraceRight) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon1) {
            tokensForParser(
                    {functionKey, semicolon, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon2) {
            tokensForParser(
                    {functionKey, identifierFunc, semicolon, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon3) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, semicolon, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon4) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, semicolon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon5) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey, semicolon,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_Good) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_WithSemicolons) {
            tokensForParser(
                    {semicolon, semicolon,
                     functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft,
                     semicolon, semicolon, semicolon,
                     curlyBraceRight,
                     semicolon, semicolon, semicolon,});

            parser();
        }

        TEST_F(ParserTestSuccess, functionDeclaration_WithSemicolonsAndReturnKeys) {
            tokensForParser(
                    {returnKey, semicolon, semicolon,
                     functionKey, identifierFunc, leftPar, rightPar, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon, semicolon, semicolon,
                     curlyBraceRight,
                     returnKey, semicolon, semicolon, semicolon,});

            parser();
        }


    }
}

#endif
