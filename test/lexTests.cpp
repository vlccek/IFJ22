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
                    "$hovno $hPvnpS $HOVNO $HOVNO $HOVNO6 $HO999VNO $hovno_kod $hovnokod_ $_hovno $6HOVNO $6hovno");
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {identifierVar, identifierVar, identifierVar, identifierVar, identifierVar, identifierVar,
                            identifierVar, identifierVar,});

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");

        }

        TEST_F(LexTestSimple, function_names) {
            auto text = PhpPrologString(
                    "hovno() hovnokodjesuper() hocnoko55d() hovnokod556655() jehovnokodsuper_skeropes() _hovno()"
                    "6hovno() skeropes_() else() float() function() if() int() null() return() string()"
                    "void(), while()");
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {identifierFunc, identifierFunc, identifierFunc, identifierFunc, identifierFunc,
                            identifierFunc});

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");
            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(lexEr), ".*");

        }

        TEST_F(LexTestSimple, function_param) {
            auto text = PhpPrologString(
                    "hovno(string $a) hovno(int $a) hovno(float $a) "
                    "hovno(string? $a) hovno(int? $a) hovno(float? $a) "
                    "hovno(string $a, float $a) hovno(string $a, int $a, float $a)"
            );
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {
                                   identifierFunc, leftPar, stringDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, intDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, floatDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, stringNullDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, intNullDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, floatNullDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, stringDat, identifierVar, floatDat, identifierVar, rightPar,
                                   identifierFunc, leftPar, stringDat, identifierVar, floatDat, identifierVar,
                                   floatDat, // \n
                                   identifierVar, rightPar
                           });

        }

        TEST_F(LexTestSimple, string_lit) {
            auto text = PhpPrologString(
                    R"("Hovnodkod" "" "\n \t \034" "Ahoj\n\"Sve'te \\\034 \x00")");
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {stringLiteral, stringLiteral, stringLiteral});
        }

        TEST_F(LexTestSimple, comment) {
            // jestli ti neprojde tak gl :)
            auto text = PhpPrologString(
                    "//skeropes"
                    "int "
                    "// int $a \n"
                    "// */ \t \t                                              //hovno \n"
                    "\t \t \t                                                "
                    "/*hovnokod******************je///////////* super tvl*/ int ");
            FILE *fp = prepareFile(text.get());
            assertTokensEq(fp,
                           {intDat, intDat});
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

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, stringDat, identifierVar,
                                           stringDat,
                                           identifierVar, rightPar, colon, stringDat, curlyBraceRight,
                                           returnKey, identifierVar, concatenationOp, stringLiteral,
                                           curlyBraceLeft};
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

        TEST_F(LexTestSimple, varTypeKeywords) {
            auto text = PhpPrologString("float int string"
                                        "?float ?int ?string");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, intDat, stringDat,
                                floatNullDat, intNullDat, stringNullDat});
        }

        TEST_F(LexTestSimple, functionParams) {
            auto text = PhpPrologString("function bar(string $param) : string {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, intDat, stringDat,
                                floatNullDat, intNullDat, stringNullDat});
        }

        TEST_F(LexTestSimple, functionReturnValue1) {
            auto text = PhpPrologString("function bar() : string {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, stringDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue2) {
            auto text = PhpPrologString("function asfasflasflasdnflasdnf() : ?string {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, stringNullDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue3) {
            auto text = PhpPrologString("function afdsf___ds() : int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, intDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue4) {
            auto text = PhpPrologString("function bar() : ?int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, intNullDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue5) {
            auto text = PhpPrologString("function bar() : float {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, floatDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue6) {
            auto text = PhpPrologString("function bar() : ?float {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, floatNullDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier0) {
            auto text = PhpPrologString("function bar() : int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, intDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier1) {
            auto text = PhpPrologString("function __4554() : int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, intDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier2) {
            auto text = PhpPrologString("function _ASDF48() : int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, intDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier3) {
            auto text = PhpPrologString("function _ASDF48() : int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                                , colon, intDat, curlyBraceLeft});
        }

        TEST_F(LexTestEdgeCase, functionIdentifier4) {
            // identifier cannot start with digits
            auto text = PhpPrologString("function 48sadf_ds() : int {\n");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, functionIdentifier5) {
            // identifier cannot start with digits
            auto text = PhpPrologString("function 6_6() : int {\n");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestSimple, functionIdentifier6) {
            auto text = PhpPrologString("function AZaz__xy() : int {\n");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar
                    , colon, intDat, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, expressionsSimple0) {
            auto text = PhpPrologString(R"("susenky" + 14)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {stringDat, plusOp, intDat});
        }

        TEST_F(LexTestSimple, expressionsSimple1) {
            auto text = PhpPrologString(R"((15 + 36) * "susenky")");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, intDat, plusOp, leftPar,
                                multiplicationOp, stringDat});
        }

        TEST_F(LexTestSimple, expressionsSimple2) {
            auto text = PhpPrologString(R"((((14+69))))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, leftPar, leftPar,
                                intDat, plusOp, intDat,
                                rightPar, rightPar, rightPar, });
        }

        TEST_F(LexTestSimple, expressionsSimple3) {
            auto text = PhpPrologString(R"((((14+69))))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, leftPar, leftPar,
                                intDat, plusOp, intDat,
                                rightPar, rightPar, rightPar, });
        }

        TEST_F(LexTestSimple, expressionsSimple4) {
            auto text = PhpPrologString(R"(((("696996"/"696969"))))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, leftPar, leftPar,
                                stringDat, divisionOp, stringDat,
                                rightPar, rightPar, rightPar, });
        }

        TEST_F(LexTestSimple, expressionsSimple5) {
            auto text = PhpPrologString(R"(((("696996"/"696969"))))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, leftPar, leftPar,
                                stringDat, divisionOp, stringDat,
                                rightPar, rightPar, rightPar, });
        }

        TEST_F(LexTestSimple, expressionsSimple6) {
            auto text = PhpPrologString(R"(15.159e69*0.0e69+        6969699.698E99/)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {floatDat, multiplicationOp,
                                floatDat, plusOp,
                                floatDat, divisionOp});
        }

        TEST_F(LexTestSimple, expressionsSimple7) {
            auto text = PhpPrologString(R"("69696"* /* jak se mas */ / "6969" \n * "6969"/)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {stringDat, multiplicationOp, divisionOp,
                                stringDat, multiplicationOp,
                                stringDat, divisionOp});
        }

        TEST_F(LexTestSimple, expressionsVariable4) {
            auto text = PhpPrologString(R"(($_67/$cookie*$bruh+$____fuuuh-$huh)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierVar, divisionOp,
                                identifierVar, multiplicationOp,
                                identifierVar, plusOp,
                                identifierVar, minusOp});
        }

        TEST_F(LexTestSimple, expressionsVariable2) {
            auto text = PhpPrologString(R"(($_67/$cookie*$bruh+$____fuuuh-$huh)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierVar, divisionOp,
                                identifierVar, multiplicationOp,
                                identifierVar, plusOp,
                                identifierVar, minusOp});
        }

        TEST_F(LexTestSimple, expressionsVariable3) {
            auto text = PhpPrologString(R"(((($howyoudoin))))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, leftPar, leftPar,
                                identifierVar,
                                rightPar, rightPar, rightPar, });
        }

        TEST_F(LexTestAdvanced, expressionsExtended) {
            auto text = PhpPrologString(R"(($_69 + 69 /(0) * ($ABCD * $bruh)))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, identifierVar, plusOp, intDat,
                                divisionOp, leftPar, intDat, rightPar,
                                multiplicationOp, leftPar, identifierVar, multiplicationOp,
                                identifierVar, rightPar, rightPar});
        }

        TEST_F(LexTestAdvanced, expressionsExtended1) {
            auto text = PhpPrologString(R"((("kokos")) + 67 * (($cookie)))");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {leftPar, leftPar, stringDat, rightPar, rightPar,
                                plusOp, intDat, multiplicationOp,
                                leftPar, leftPar, identifierVar, rightPar, rightPar});
        }

        TEST_F(LexTestEdgeCase, weirdStringWithComments) {
            auto text = PhpPrologString(R"("\"\"\"\"\"susenky\"\"\"\"\multiplicationOp"" / /*Huh how r u**// "\"\"huhu\"\"")");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { stringLiteral, divisionOp, divisionOp, stringLiteral });
        }
        TEST_F(LexTestEdgeCase, expression) {
            // Two operands in exp
            auto text = PhpPrologString("/*");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression1) {
            // Two operands in exp
            auto text = PhpPrologString("/+");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression2) {
            // More operands in exp
            auto text = PhpPrologString("+/*-");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression3) {
            auto text = PhpPrologString(R"($cookie + 998 / "bruhie" *-)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression4) {
            auto text = PhpPrologString(R"($cookie * $bruh/// This should not return divisionOp)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { identifierVar, multiplicationOp, identifierVar});
        }

        TEST_F(LexTestEdgeCase, expression5) {
            auto text = PhpPrologString(R"(/* this is comment*/ / / /)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {divisionOp, divisionOp, divisionOp, });
        }

        TEST_F(LexTestSimple, prologAtTheEnd1) {
            auto text = PhpPrologString(R"(1 ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { intDat});
        }

        TEST_F(LexTestSimple, prologAtTheEnd2) {
            auto text = PhpPrologString(R"(1 ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { intDat});
        }

        TEST_F(LexTestSimple, prologAtTheEnd3) {
            auto text = PhpPrologString(R"(1 ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { intDat});
        }

        TEST_F(LexTestSimple, prologAtTheEnd4) {
            auto text = PhpPrologString(R"(//msg @Fado if you read this \n 1 ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { intDat});
        }

        TEST_F(LexTestEdgeCase, prologAtTheEnd5) {
            auto text = PhpPrologString(R"(//how much wood would a woodchuck chuck \n\n\n ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { ending});
        }

        TEST_F(LexTestSimple, prologAtTheEnd6) {
            auto text = PhpPrologString(R"(?int ?float ?string ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { intNullDat, floatNullDat, stringNullDat, ending});
        }

        TEST_F(LexTestSimple, prologAtTheEndIsNotEnd) {
            auto text = PhpPrologString(R"(?int ?float ?string ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { intNullDat, floatNullDat, stringNullDat, ending});
        }

        TEST_F(LexTestSimple, prologAtTheEndIsNotEnd1) {
            auto text = PhpPrologString(R"("kok" 45 56.5 ?>)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {stringLiteral, decimalLiteral, floa});
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd) {
            auto text = PhpPrologString(R"(?int ?float ?string ?> ?int ?float ?string)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd1) {
            auto text = PhpPrologString(R"(?int ?float ?string ?> ?int ?float ?string)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd2) {
            auto text = PhpPrologString(R"(?int ?float ?string ?>>)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd3) {
            auto text = PhpPrologString(R"(?int ?float ?string ?> /* not even comment is legit here*/ )");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd4) {
            auto text = PhpPrologString(R"(//how dare you ?> ">>>> hehe <<<<<")");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd5) {
            auto text = PhpPrologString(R"(?int ?float ?string ?> ?>)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd6) {
            auto text = PhpPrologString(R"(int float $nothing_interesting ?>\t)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd7) {
            auto text = PhpPrologString(R"(int float $nothing_interesting ?>\n)");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd8) {
            auto text = PhpPrologString(R"(int float $nothing_interesting ?> )");
            FILE *fp = prepareFile(text.get());

            ASSERT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, whiteSpaces1) {
            auto text = PhpPrologString(R"(\t\t\n65\n67\t67)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { integerLiteral, integerLiteral, integerLiteral});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces2) {
            auto text = PhpPrologString(R"(\nn)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces3) {
            auto text = PhpPrologString(R"(\nn)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces4) {
            auto text = PhpPrologString(R"(\tn)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces5) {
            auto text = PhpPrologString(R"( n)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces6) {
            auto text = PhpPrologString(R"(\n\t    \t\n\n\n huhu)");
            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces7) {
            auto text = PhpPrologString(R"(\n\t    \t\n\n\n  )");

            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { ending });
        }

        TEST_F(LexTestEdgeCase, whiteSpaces8) {
            auto text = PhpPrologString(R"(\nn\tt t)");

            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { identifierFunc, identifierFunc, identifierFunc, });
        }

        TEST_F(LexTestEdgeCase, whiteSpaces9) {
            auto text = PhpPrologString(R"(\   n)");

            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { ending });
        }

        TEST_F(LexTestEdgeCase, whiteSpaces10) {
            auto text = PhpPrologString(R"("\n\t\t\n  \t\n"?>)");

            FILE *fp = prepareFile(text.get());

            assertTokensEq(fp, { stringLiteral });
        }


        // Mby testovat keywords v definici funkci
        // Asi by to chtelo slozite funkce
        // string literal testy
        //
    }// namespace stack
}// namespace ifj22
