#include "gtest/gtest.h"

#ifndef PARSER_TEST
#define PARSER_TEST

// Hide main

// Hide the io function since this will segfault in testing
extern "C" {
#include "../parser.h" // DON'T TOUCH THIS!
#include "../parser.c" // DON'T TOUCH THIS!
#include "../dynstring.h"

#include <stdio.h>
};


#if(DEBUG)
#define ASSERT_EXIT_CODE(statement, number) \
        statement;
#else
#define ASSERT_EXIT_CODE(statement, number) \
        ASSERT_EXIT(statement, ::testing::ExitedWithCode(number), ".*");
#endif

#define ASSERT_EXIT_SYNTAX(statement) \
        ASSERT_EXIT_CODE(statement, ERR_SYNTAX);

#define ASSERT_NO_EXIT(statement)\
        ASSERT_EXIT_CODE({{ statement ;} ::exit(EXIT_SUCCESS); }, EXIT_SUCCESS);

namespace ifj22 {
    namespace parsertest {
        class ParserTest : public ::testing::Test {
        protected:
            std::vector<token_t> *tokens;
            token_t endingToken = {ending, {}, 0, 0};
            int uID = 0;
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
                            newToken.data.valueString = dstrInitChar(
                                    (std::string("hovnokod") + std::to_string(uID)).c_str());
                            break;
                    }
                    tokens->push_back(newToken);
                }
                tokens->push_back(endingToken);
                testTokens = tokens->data();
            }

            void SetUp() override {
                tokens = new std::vector<token_t>;
            }

            void TearDown() override {
                delete tokens;
            }

            void *genLit(const char *s){
                token_t t = {stringLiteral, dstrInitChar(s), 0, 0};
                tokens->push_back(t);
            }

            void *genLit(int i){
                token_t t = {floatLiteral, nullptr, 0, 0};
                t.data.valueFloat = i;
                tokens->push_back(t);
            }

            void *genLit(float f){
                token_t t = {floatLiteral, nullptr, 0, 0};
                t.data.valueFloat = f;
                tokens->push_back(t);
            }

            void *genVar(const char *s ){
                token_t t = {stringLiteral, dstrInitChar(s), 0, 0};
                tokens->push_back(t);
            }

            void *genFce(const char *s){
                token_t t = {stringLiteral, dstrInitChar(s), 0, 0};
                tokens->push_back(t);
            }

        };

        class ParserTestSyntaxError : public ParserTest {
        };

        class ParserTestSuccess : public ParserTest {
        };

        TEST_F(ParserTestSuccess, initialTest) {
            tokensForParser({ending});
            ASSERT_NO_EXIT(parser());
        }

        TEST_F(ParserTestSyntaxError, initialTest2) {
            tokensForParser({leftPar, rightPar, ending});

            ASSERT_EXIT_SYNTAX(parser();)
        }

        // region basic_op
        TEST_F(ParserTestSuccess, concatenationOp_basic) {
            tokensForParser({stringLiteral, concatenationOp, identifierVar, ending});

            ASSERT_NO_EXIT(parser())
        }
        TEST_F(ParserTestSuccess, plusOP) {
            tokensForParser({stringLiteral, plusOp, identifierVar, ending});

            ASSERT_NO_EXIT(parser())
        }
        TEST_F(ParserTestSuccess, minusOP) {
            tokensForParser({stringLiteral, minusOp, identifierVar, ending});

            ASSERT_NO_EXIT(parser())
        }
        TEST_F(ParserTestSuccess, multipl) {
            tokensForParser({stringLiteral, multiplicationOp, identifierVar, ending});

            ASSERT_NO_EXIT(parser())
        }
        TEST_F(ParserTestSuccess, div) {
            tokensForParser({stringLiteral, divisionOp, identifierVar, ending});

            ASSERT_NO_EXIT(parser())
        }
        // endregion

        // region pars_op
        TEST_F(ParserTestSyntaxError, concatenationOp_par) {
            tokensForParser(
                    {stringLiteral, concatenationOp, leftPar, identifierVar, concatenationOp, identifierVar, rightPar,
                     ending});

            ASSERT_EXIT_SYNTAX(parser())
        }
        TEST_F(ParserTestSyntaxError, add_par) {
            tokensForParser(
                    {stringLiteral, plusOp, leftPar, identifierVar, plusOp, identifierVar, rightPar,
                     ending});

            ASSERT_EXIT_SYNTAX(parser())
        }
        TEST_F(ParserTestSyntaxError, multiOp_par) {
            tokensForParser(
                    {stringLiteral, multiplicationOp, leftPar, identifierVar, multiplicationOp, identifierVar, rightPar,
                     ending});

            ASSERT_EXIT_SYNTAX(parser())
        }
        TEST_F(ParserTestSyntaxError, divOP_par) {
            tokensForParser(
                    {stringLiteral, divisionOp, leftPar, identifierVar, divisionOp, identifierVar, rightPar,
                     ending});

            ASSERT_EXIT_SYNTAX(parser())
        }

        //end region


        TEST_F(ParserTestSuccess, functionDeclaration) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        //endregion

        TEST_F(ParserTestSuccess, functionDeclaration_returnVoid) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon,
                     curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnInMainBody) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight,
                     returnKey, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnEverywhere) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon,
                     curlyBraceRight,
                     returnKey, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnOnly) {
            tokensForParser(
                    {returnKey, semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_returnOnly_ManyTimes) {
            tokensForParser(
                    {returnKey, semicolon,
                     returnKey, semicolon,
                     returnKey, semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_SemicolonOnly) {
            tokensForParser(
                    {semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_ManySemicolons) {
            tokensForParser(
                    {semicolon, semicolon, semicolon, semicolon, semicolon,});
            ASSERT_NO_EXIT(parser())
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
                    {functionKey, identifierFunc, leftPar, rightPar, colon,
                     curlyBraceLeft, curlyBraceRight});

           ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoFunctionKey) {
            tokensForParser(
                    {identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser());
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NoidentifierFunc) {
            tokensForParser(
                    {functionKey, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NocurlyBraceLeft) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_NocurlyBraceRight) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleFunctionKey) {
            tokensForParser(
                    {functionKey, functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleIdentifierFunc) {
            tokensForParser(
                    {functionKey, identifierFunc, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleLeftPar) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleRightPar) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleReturnType) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleCurlyBraceLeft) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_DoubleCurlyBraceRight) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon1) {
            tokensForParser(
                    {functionKey, semicolon, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon2) {
            tokensForParser(
                    {functionKey, identifierFunc, semicolon, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon3) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, semicolon, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon4) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, semicolon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_RandomSemicolon5) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, voidKey, semicolon,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_Good) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_WithSemicolons) {
            tokensForParser(
                    {semicolon, semicolon,
                     functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     semicolon, semicolon, semicolon,
                     curlyBraceRight,
                     semicolon, semicolon, semicolon,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_WithSemicolonsAndReturnKeys) {
            tokensForParser(
                    {returnKey, semicolon, semicolon,
                     functionKey, identifierFunc, leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon, semicolon, semicolon,
                     curlyBraceRight,
                     returnKey, semicolon, semicolon, semicolon,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_FunctionCall) {
            genLex(functionKey);
            genFce("foo");
            tokensForParser(
                    { leftPar, rightPar, colon, voidKey,
                     curlyBraceLeft,
                     returnKey, semicolon,
                     curlyBraceRight,});

            genFce("foo");
            tokensForParser({leftPar, rightPar, semicolon});
            ASSERT_NO_EXIT(parser())
        }


    }
}

#endif
