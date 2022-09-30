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
            PhpPrologString(const char *text) {
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


    }// namespace stack
}// namespace ifj22
