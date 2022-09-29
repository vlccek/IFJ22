#include "gtest/gtest.h"
#include <string>
// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../lex.h"

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

            FILE *prerpareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                return fileForTest;
            }

            static void runTest(const std::vector<lexType>& checkTokens){
                for (auto i: checkTokens) {
                    token_t t = getToken(fp);
                    ASSERT_EQ(t.type, i);
                }
            }
        };

        class PhpPrologString {
        private:
            std::string value;

            void prependProlog(){
                value.insert(0, "<?php\n"
                                "declare(strict_types=1);\n");
            }

        public:
            PhpPrologString(const char *text){
                value = std::string(text);
                prependProlog();
            }
            const char *get(){
                return value.c_str();
            }

        };


        TEST_F(lexTest, prolog_unallowChars) {

            char text[] = "randomcharejjeojeoeojejojeojeojeojeoejeoj"
                          "<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prerpareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");

        }

        TEST_F(lexTest, prolog_unallowChars2) {
            char text[] = "<?phpjenej\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prerpareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");

        }

        TEST_F(lexTest, prolog_unallowChars3) {
            char text[] = "<?php\n"
                          "a = 10;"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prerpareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");

        }


        TEST_F(lexTest, function_declare) {

            char text[] = "<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prerpareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, stringDat,
                                           identifierVar, rightPar, colon, stringDat, curlyBraceRight,
                                           returnKey, identifierFunc, identifierVar, semiColon, curlyBraceLeft};
            for (auto i: tokens) {
                token_t t = getToken(fp);
                ASSERT_EQ(t.type, i);
            }
        }




        TEST_F(lexTest, keywords_test) {
            auto text = PhpPrologString("else float function if int null return"
                                        "string void while");
            FILE *fp = prerpareFile(text.get());

            runTest({elseKey, floatDat, functionKey, ifKey, intDat, nullKey,
                     returnKey, stringDat, voidKey, whileKey});
        }
    }// namespace stack
}// namespace ifj22
