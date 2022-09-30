#include "gtest/gtest.h"
#include <string>
// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../lex.h"
#include "../common.h"

#include <stdio.h>
};

class PhpPrologString {
private:
    std::string value;

    void prependProlog() {
        value.insert(0, "<?php\n"
                        "declare(strict_types=1);\n");
    }

public:
    PhpPrologString(const char *text) {
        value = std::string(text);
        prependProlog();
    }

    const char *get() {
        return value.c_str();
    }

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

            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                return fileForTest;
            }

            void assertTokensEq(FILE *fp, const std::vector<lexType> &checkTokens) {
                for (auto i: checkTokens) {
                    token_t t = getToken(fp);
                    ASSERT_STREQ(getTerminalName(t.type), getTerminalName(i));
                }
            }
        };

        class LexTestSimple : public lexTest {

        };

        TEST_F(LexTestSimple, variable) {
            auto text = PhpPrologString(
                    "$hovno $hPvnpS $HOVNO $HOVNO $HOVNO6 $HO999VNO $hovno_kod $hovnokod_$6HOVNO $6hovno");
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {identifierVar, identifierVar, identifierVar, identifierVar, identifierVar, identifierVar,
                            identifierVar,});

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");

        }

        TEST_F(LexTestSimple, function_names) {
            auto text = PhpPrologString(
                    "hovno() hovnokodjesuper() hocnoko55d() hovnokod556655() jehovnokodsuper_skeropes() "
                    "6hovno() skeropes_()");
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {identifierFunc, identifierFunc, identifierFunc, identifierFunc, identifierFunc});

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");

        }

        TEST_F(LexTestSimple, function_param) {
            auto text = PhpPrologString(
                    "hovno(string $a) hovno(int $a) hovno(float $a) "
                    "hovno(string $a, float $a) hovno(string $a, int $a, float $a)"
            );
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {identifierFunc, leftPar, stringDat, identifierVar, rightPar,
                            identifierFunc, leftPar, intDat, identifierVar, rightPar,
                            identifierFunc, leftPar, floatDat, identifierVar, rightPar,
                            identifierFunc, leftPar, stringDat, identifierVar, floatDat, identifierVar, rightPar,
                            identifierFunc, leftPar, stringDat, identifierVar, floatDat, identifierVar, floatDat, // \n
                            identifierVar, rightPar
                           });

        }

        class LexTestAdvanced : public lexTest {
        };

        class LexTestEdgeCase : public lexTest {
        };

        class LexTestTokenData : public lexTest {
        };

        class PhpPrologString {
        private:
            std::string value;

            void prependProlog() {
                value.insert(0, "<?php\n"
                                "declare(strict_types=1);\n");
            }

        public:
            PhpPrologString(const char *text = "") {
                value = std::string(text);
                prependProlog();
            }

            const char *get() {
                return value.c_str();
            }

        };


        TEST_F(LexTestAdvanced, prolog_unallowChars) {

            char text[] = "randomcharejjeojeoeojejojeojeojeojeoejeoj"
                          "<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);;

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");

        }

        TEST_F(LexTestAdvanced, prolog_unallowChars2) {
            char text[] = "<?phpjenej\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");

        }

        TEST_F(LexTestAdvanced, prolog_unallowChars3) {
            char text[] = "<?php\n"
                          "a = 10;"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");

        }

        TEST_F(LexTestAdvanced, function_declare_more_args) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);\n"
                          "function concat(string $x, string $y): string {"
                          "return $x . \" \" . $y;\n"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, stringDat, identifierVar, stringDat,
                                           identifierVar, rightPar, colon, stringDat, curlyBraceRight,
                                           returnKey, identifierVar, concatenationOp, stringLiteral, curlyBraceLeft};
            for (auto i: tokens) {
                token_t t = getToken(fp);
                ASSERT_EQ(t.type, i);
            }

        }

        TEST_F(LexTestAdvanced, function_declare_empty_param) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);\n"
                          "function concat():string {"
                          "return \" \" \n;"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, rightPar, colon, stringDat,
                                           curlyBraceRight,
                                           returnKey, stringLiteral, semicolon,
                                           curlyBraceLeft};
            for (auto i: tokens) {
                token_t t = getToken(fp);
                ASSERT_EQ(t.type, i);
            }


        }

        TEST_F(LexTestAdvanced, function_declare_name) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);\n"
                          "function 2concat():string {"
                          "return \" \" \n;"
                          "}";
            FILE *fp = prepareFile(text);

            getToken(fp); // function key
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
        }

        TEST_F(LexTestAdvanced, function_declare_name_with_num) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);\n"
                          "function concat2():string {"
                          "return \" \" \n;"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, rightPar, colon, stringDat,
                                           curlyBraceRight,
                                           returnKey, stringLiteral, semicolon,
                                           curlyBraceLeft};

            for (auto i: tokens) {
                token_t t = getToken(fp);
                ASSERT_EQ(t.type, i);
            }
        }

        TEST_F(LexTestAdvanced, function_declare) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, stringDat,
                                           identifierVar, rightPar, colon, stringDat, curlyBraceRight,
                                           returnKey, identifierFunc, identifierVar, semicolon, curlyBraceLeft};
            for (auto i: tokens) {
                token_t t = getToken(fp);
                ASSERT_EQ(t.type, i);
            }
        }


        TEST_F(LexTestSimple, keywords_test) {
            auto text = PhpPrologString("else float function if int null return"
                                        "string void while");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {elseKey, floatDat, functionKey, ifKey, intDat, nullKey,
                                returnKey, stringDat, voidKey, whileKey});
        }

        TEST_F(LexTestSimple, condition) {
            auto text = PhpPrologString("if (1) { 1; } else { 7; }");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {ifKey, leftPar, integerLiteral, rightPar,
                                curlyBraceLeft, intDat, semicolon, curlyBraceRight,
                                elseKey, curlyBraceLeft, intDat, semicolon, curlyBraceRight});
        }

        // literál je tvořen celou a desetinnou částí,
        TEST_F(LexTestSimple, floatTest) {
            auto text = PhpPrologString("0.0 00000.0000 1.0005 61561615.15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }
        // nebo celou částí a exponen-tem,
        TEST_F(LexTestSimple, floatTestExponents1) {
            auto text = PhpPrologString("0.e0 00000.e0000 1.e0005 61561615.e15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestSimple, floatTestExponents2) {
            auto text = PhpPrologString("0.e+0 00000.e+0000 1.e+0005 61561615.e+15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestSimple, floatTestExponents3) {
            auto text = PhpPrologString("0.e-0 00000.e-0000 1.e-0005 61561615.e-15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestSimple, floatTestExponents4) {
            auto text = PhpPrologString("0.E0 00000.E0000 1.E0005 61561615.E15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestSimple, floatTestExponents5) {
            auto text = PhpPrologString("0.E+0 00000.E+0000 1.E+0005 61561615.E+15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestSimple, floatTestExponents6) {
            auto text = PhpPrologString("0.E-0 00000.E-0000 1.E-0005 61561615.E-15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }
        // nebo celou a desetinnou částí a exponentem.
        TEST_F(LexTestSimple, floatTestDecimalExponents1) {

            auto text = PhpPrologString("0.0e0 00000.15e0000 1.8498e0005 61561615.0000e15161561 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestSimple, floatTestDecimalExponents2) {

            auto text = PhpPrologString("0.0e0 0.0E0 0.0e-1 "
                                        "0.0e+1 0.0E+1 0.0E-1 ");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, floatDat, floatDat,
                                floatDat, floatDat, floatDat,});
        }

        TEST_F(LexTestEdgeCase, floatFail1) {
            // desetinna cast nesmi byt pradna
            auto text = PhpPrologString("0.e44");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail2) {
            // desetinna cast nesmi byt pradna
            auto text = PhpPrologString("0.e44");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");

        }

        TEST_F(LexTestEdgeCase, floatFail3) {
            // cela cast nesmi byt pradna
            auto text = PhpPrologString(".5e44");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail4) {
            // +- nelze
            auto text = PhpPrologString("0.0e+-44");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }


        TEST_F(LexTestEdgeCase, floatFail5) {
            // -+ nelze
            auto text = PhpPrologString("0.0e-+44");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail6) {
            // expo cast nesmí byt pradna
            auto text = std::string(PhpPrologString().get()) + std::to_string(INTMAX_MAX);
            FILE *fp = prepareFile(text.c_str());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }


    }// namespace stack
}// namespace ifj22
