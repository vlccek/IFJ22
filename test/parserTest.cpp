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

            void tokensForParserNoEnding(const std::vector<lexType> &lexTypes) {
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
                testTokens = tokens->data();
            }

            /**bypass getToken function,
             *
             * lyxtype tokens for parse function
             */
            void tokensForParser(const std::vector<lexType> &lexTypes) {
                tokensForParserNoEnding(lexTypes);
                tokens->push_back(endingToken);
                testTokens = tokens->data();
            }

            void SetUp() override {
                tokens = new std::vector<token_t>;
            }

            void TearDown() override {
                delete tokens;
            }

            void genLit(int i) {
                token_t t = {floatLiteral, nullptr, 0, 0};
                t.data.valueFloat = i;
                tokens->push_back(t);
            }

            void genLit(double f) {
                token_t t = {floatLiteral, nullptr, 0, 0};
                t.data.valueFloat = f;
                tokens->push_back(t);
            }

            void genVar(const char *s) {
                token_t t = {identifierVar, dstrInitChar(s), 0, 0};
                tokens->push_back(t);
            }

            void genFce(const char *s) {
                token_t t = {identifierFunc, dstrInitChar(s), 0, 0};
                tokens->push_back(t);
            }

            void genLex(lexType type) {
                token_t t = {type, {}, 0, 0};
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

        TEST_F(ParserTestSuccess, functionDeclaration_GoodReturnString) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, stringKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodReturnInt) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, intKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodReturnFloat) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, floatKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodReturnFloatReturning) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, floatKey,
                     curlyBraceLeft,
                     returnKey, floatLiteral, semicolon,
                     curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodReturnStringReturning) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, stringKey,
                     curlyBraceLeft,
                     returnKey, stringLiteral, semicolon,
                     curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodReturnIntReturning) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar, rightPar, colon, intKey,
                     curlyBraceLeft,
                     returnKey, integerLiteral, semicolon,
                     curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, ProgramBody_ReturnValues) {
            tokensForParser(
                    {returnKey, integerLiteral, semicolon,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSyntaxError, ProgramBody_ReturnValues1) {
            tokensForParser(
                    {returnKey, returnKey, integerLiteral, semicolon,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, ProgramBody_ReturnValues2) {
            tokensForParser(
                    {equals, returnKey, integerLiteral, semicolon,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSuccess, ProgramBody_ReturnValues3) {
            tokensForParser(
                    {returnKey, integerLiteral, semicolon,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, ProgramBody_ReturnValues1) {
            tokensForParser(
                    {returnKey, floatLiteral, semicolon,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, ProgramBody_ReturnValues2) {
            tokensForParser(
                    {returnKey, stringLiteral, semicolon,});

            ASSERT_NO_EXIT(parser())
        }

        // parametry funkcí
        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithParams1) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     stringKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithParams2) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     stringNullKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithParams3) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithParams4) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intNullKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithParams5) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     floatKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithParams6) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     floatNullKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithManyParams1) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     stringKey, identifierVar, comma, stringKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithManyParams2) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     stringKey, identifierVar, comma, intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithManyParams3) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intNullKey, identifierVar, comma, intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithManyParams4) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar, comma, intKey, identifierVar, comma, intKey, identifierVar, comma,
                     intKey, identifierVar, comma, intKey, identifierVar, comma, intKey, identifierVar, comma,
                     intKey, identifierVar, comma, intKey, identifierVar, comma, intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodWithManyParamsAllPosibilities) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar, comma, stringKey, identifierVar, comma, floatKey, identifierVar, comma,
                     intNullKey, identifierVar, comma, stringNullKey, identifierVar, comma, floatNullKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_ParamsEndingComma) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar, comma,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_ParamsStartingComma) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     comma, intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_ParamsMissingComma) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar, intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_ParamsMissingRighPar) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar, comma, intKey, identifierVar,
                     colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, functionDeclaration_ParamsDoubleComma) {
            tokensForParser(
                    {functionKey, identifierFunc, leftPar,
                     intKey, identifierVar, comma, comma, intKey, identifierVar,
                     rightPar, colon, voidKey,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_EXIT_SYNTAX(parser())
        }

        // assignment
        TEST_F(ParserTestSuccess, assignment_string) {
            tokensForParser(
                    {identifierVar, equals, stringLiteral, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_int) {
            tokensForParser(
                    {identifierVar, equals, integerLiteral, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_float) {
            tokensForParser(
                    {identifierVar, equals, floatLiteral, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_ReAssignment) {
            genVar("foo");
            tokensForParserNoEnding(
                    {equals, floatLiteral, semicolon});

            genVar("foo");
            genLex(equals);
            genLit(14.9);
            tokensForParser({semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_ReAssignmentRndSemicolons) {
            genVar("foo");
            tokensForParserNoEnding(
                    {equals, floatLiteral, semicolon, semicolon, semicolon, semicolon});

            genVar("foo");
            genLex(equals);
            genLit(14.9);
            tokensForParser({semicolon, semicolon, semicolon, semicolon,});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_VarToVa1) {
            genVar("foo");
            tokensForParserNoEnding(
                    {equals, floatLiteral, semicolon});

            genVar("boo");
            genLex(equals);
            genVar("foo");
            tokensForParser({semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_VarToVar2) {
            genVar("foo");
            tokensForParserNoEnding(
                    {equals, integerLiteral, semicolon});

            genVar("boo");
            genLex(equals);
            genVar("foo");
            tokensForParser({semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, assignment_VarToVar3) {
            genVar("foo");
            tokensForParserNoEnding(
                    {equals, stringLiteral, semicolon});

            genVar("boo");
            genLex(equals);
            genVar("foo");
            tokensForParser({semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection1) {
            tokensForParser(
                    {identifierVar, equals, semicolon, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection2) {
            tokensForParser(
                    {identifierVar, equals, elseKey, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection3) {
            tokensForParser(
                    {identifierVar, equals, whileKey, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection4) {
            tokensForParser(
                    {identifierVar, equals, curlyBraceLeft, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection5) {
            tokensForParser(
                    {identifierVar, equals, curlyBraceRight, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection6) {
            tokensForParser(
                    {identifierVar, equals, comma, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection7) {
            tokensForParser(
                    {identifierVar, equals, colon, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection8) {
            tokensForParser(
                    {identifierVar, equals, divisionOp, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection9) {
            tokensForParser(
                    {identifierVar, equals, intNullKey, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection10) {
            tokensForParser(
                    {identifierVar, equals, intKey, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, assignment_RandomLexTypeInjection11) {
            tokensForParser(
                    {identifierVar, equals, floatKey, floatLiteral, semicolon});

            ASSERT_EXIT_SYNTAX(parser())
        }
        // if else

        TEST_F(ParserTestSuccess, ConditionSimple) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, ConditionSimple2) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, ConditionSimple3) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, ConditionSimple4) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, Condition_MoreConditions) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,

                     ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, Condition_ConditionInCondition) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar, curlyBraceLeft,
                            //inner if
                     ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,

                     curlyBraceRight,
                    });

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_NoCurlyBraceLeft) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_NoCurlyBraceRight) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_NoElse) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }


        TEST_F(ParserTestSyntaxError, Condition_NoLeftPar) {
            tokensForParser(
                    {ifKey, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_NoRightPar) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection1) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar, functionKey,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection2) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, functionKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection3) {
            tokensForParser(
                    {ifKey, intKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection4) {
            tokensForParser(
                    {ifKey, floatKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection5) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection6) {
            tokensForParser(
                    {ifKey, ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection7) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, comma, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection8) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection9) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, Condition_RandomLexTypeInjection10) {
            tokensForParser(
                    {ifKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     elseKey, curlyBraceLeft, curlyBraceRight, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        // while
        TEST_F(ParserTestSuccess, WhileSimple1) {
            tokensForParser(
                    {whileKey, leftPar, stringLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, WhileSimple2) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, While_MultipleTimes) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, While_InWhile) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar, curlyBraceLeft,
                            // inner while
                     whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     curlyBraceRight,
                    });

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, While_InWhileInWhile) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar, curlyBraceLeft,
                            // inner while
                     whileKey, leftPar, integerLiteral, rightPar, curlyBraceLeft,
                            // double inner while
                     whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,
                     curlyBraceRight,
                     curlyBraceRight,
                    });

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection1) {
            tokensForParser(
                    {whileKey, equals, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection2) {
            tokensForParser(
                    {whileKey, whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection3) {
            tokensForParser(
                    {whileKey, leftPar, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection4) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection5) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection6) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection7) {
            tokensForParser(
                    {whileKey, ending, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection8) {
            tokensForParser(
                    {whileKey, semicolon, leftPar, integerLiteral, rightPar,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection9) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar, plusOp,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        TEST_F(ParserTestSyntaxError, While_RandomLexTypeInjection10) {
            tokensForParser(
                    {whileKey, leftPar, integerLiteral, rightPar, comma,
                     curlyBraceLeft, returnKey, semicolon, curlyBraceRight,});

            ASSERT_EXIT_SYNTAX(parser())
        }

        // Assignment without id
        TEST_F(ParserTestSuccess, AssignmentWithoutId1) {
            tokensForParser(
                    {floatLiteral, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, AssignmentWithoutId2) {
            tokensForParser(
                    {integerLiteral, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, AssignmentWithoutId3) {
            tokensForParser(
                    {stringLiteral, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, AssignmentWithoutId4) {
            genVar("kokos");
            tokensForParserNoEnding({equals, stringLiteral, semicolon});
            genVar("kokos");
            tokensForParser({semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, AssignmentWithoutId5) {
            tokensForParser(
                    {leftPar, stringLiteral, rightPar, semicolon});

            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, AssignmentWithoutId6) {
            tokensForParser(
                    {leftPar, integerLiteral, plusOp, integerLiteral, rightPar, semicolon});

            ASSERT_NO_EXIT(parser())
        }
        // function calls
        TEST_F(ParserTestSuccess, functionDeclaration_GoodFuncCall1) {
            genLex(functionKey);
            genFce("boo");
            tokensForParserNoEnding(
                    {leftPar, rightPar, colon, stringKey,
                     curlyBraceLeft,
                     returnKey, stringLiteral, semicolon, curlyBraceRight});

            genVar("hoe");
            genLex(equals);
            genFce("boo");
            tokensForParser({leftPar, rightPar, semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodFuncCall2) {
            genLex(functionKey);
            genFce("boo");
            tokensForParserNoEnding(
                    {leftPar, rightPar, colon, intKey,
                     curlyBraceLeft,
                     returnKey, integerLiteral, semicolon, curlyBraceRight});

            genVar("hoe");
            genLex(equals);
            genFce("boo");
            tokensForParser({leftPar, rightPar, semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodFuncCall3) {
            genLex(functionKey);
            genFce("boo");
            tokensForParserNoEnding(
                    {leftPar, rightPar, colon, floatKey,
                     curlyBraceLeft,
                     returnKey, floatLiteral, semicolon, curlyBraceRight});

            genVar("hoe");
            genLex(equals);
            genFce("boo");
            tokensForParser({leftPar, rightPar, semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodFuncCallParams) {
            genLex(functionKey);
            genFce("boo");
            tokensForParserNoEnding(
                    {leftPar, intKey, identifierVar, rightPar, colon, floatKey,
                     curlyBraceLeft,
                     returnKey, floatLiteral, semicolon, curlyBraceRight});

            genFce("boo");
            tokensForParser({leftPar, integerLiteral, rightPar, semicolon});
            ASSERT_NO_EXIT(parser())
        }

        TEST_F(ParserTestSuccess, functionDeclaration_GoodFuncCallParamsMore) {
            genFce("write");
            tokensForParser({leftPar, integerLiteral, comma, integerLiteral, rightPar, semicolon});
            ASSERT_NO_EXIT(parser())
        }






        // void function call
        TEST_F(ParserTestSuccess, functionDeclaration_FunctionCall) {
            genLex(functionKey);
            genFce("foo");
            tokensForParserNoEnding(
                    {leftPar, rightPar, colon, voidKey,
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
