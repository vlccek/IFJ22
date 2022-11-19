#include "gtest/gtest.h"
#include <string>
// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../lex.h"
#include "../common.h"
#include "../dynstring.h"

#include <stdio.h>
};

namespace ifj22 {
    namespace stack {
        class lexTest : public ::testing::Test {

        protected:

            FILE *fileForTest;

            void SetUp() override {
                fclose(fopen("test.txt", "w"));
                fileForTest = fopen("test.txt", "ab+");
                php = true;
                declare = true;
                endingMark = false;
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                rewind(fileForTest);
                fclose(fileForTest);
                fileForTest = fopen("test.txt", "r");
                return fileForTest;
            }

            void assertTokensEq(FILE *fp, const std::vector<lexType> &checkTokens) {
                for (auto i: checkTokens) {
                    token_t t = getToken(fp);
                    EXPECT_STREQ(getTerminalName(t.type), getTerminalName(i));
                    //EXPECT_EQ((t.type), (i));
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

        class testCorrect : public testing::TestWithParam<std::string> {
            // You can implement all the usual fixture class members here.
            // To access the test parameter, call GetParam() from class
            // TestWithParam<T>.
            FILE *fileForTest = NULL;

            void SetUp() override {
                fclose(fopen("test.txt", "w"));
                fileForTest = fopen("test.txt", "ab+");
                php = true;
                declare = true;
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

        public:
            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                rewind(fileForTest);
                fclose(fileForTest);
                fileForTest = fopen("test.txt", "r");
                return fileForTest;
            }
        };
        TEST_P(testCorrect, varTestCorrect) {

            const char *string = GetParam().c_str();
            // const std::vector<lexType> checkTokens = std::get<1>(GetParam());

            FILE *fp = prepareFile(string);

            token_t t = getToken(fp);
            // EXPECT_STREQ(getTerminalName(t.type), getTerminalName(identifierVar));
            EXPECT_EQ((t.type), identifierVar);
        }

        INSTANTIATE_TEST_SUITE_P(varTestCorrect, testCorrect, ::testing::Values("$hovno", "$hPvnpS", "$HOVNO", "$HOVNO", "$HOVNO6", "$HO999VNO", "$hovno_kod", "$hovnokod_"));


        class testCorrectFunction : public testing::TestWithParam<std::string> {
            // You can implement all the usual fixture class members here.
            // To access the test parameter, call GetParam() from class
            // TestWithParam<T>.
            FILE *fileForTest = NULL;

            void SetUp() override {
                fclose(fopen("test.txt", "w"));
                fileForTest = fopen("test.txt", "ab+");
                php = true;
                declare = true;
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

        public:
            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                rewind(fileForTest);
                fclose(fileForTest);
                fileForTest = fopen("test.txt", "r");
                return fileForTest;
            }
        };
        TEST_P(testCorrectFunction, FuncTestCorrect) {

            const char *string = GetParam().c_str();
            // const std::vector<lexType> checkTokens = std::get<1>(GetParam());

            FILE *fp = prepareFile(string);

            token_t t = getToken(fp);
            // EXPECT_STREQ(getTerminalName(t.type), getTerminalName(identifierVar));
            EXPECT_EQ((t.type), identifierFunc);
        }


        INSTANTIATE_TEST_SUITE_P(functiongTestCorrect, testCorrectFunction, ::testing::Values("hovno()", "hovnokodjesuper()", "hocnoko55d()", "hovnokod556655()", "jehovnokodsuper_skeropes()", "_hovno()"));


        class TestExiting : public testing::TestWithParam<std::string> {
            // You can implement all the usual fixture class members here.
            // To access the test parameter, call GetParam() from class
            // TestWithParam<T>.
            FILE *fileForTest;

            void SetUp() override {
                fclose(fopen("test.txt", "w"));
                fileForTest = fopen("test.txt", "ab+");
                php = true;
                declare = true;
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

        public:
            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                rewind(fileForTest);
                fclose(fileForTest);
                fileForTest = fopen("test.txt", "r");
                return fileForTest;
            }
        };

        TEST_P(TestExiting, TestExiting) {
            const char *string = GetParam().c_str();
            // const std::vector<lexType> checkTokens = std::get<1>(GetParam());

            FILE *fp = this->prepareFile(string);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
            //EXPECT_EQ((t.type), (i));
        }


        INSTANTIATE_TEST_SUITE_P(variableTest, TestExiting, ::testing::Values("$6HOVNO", "$6hovno"));
        INSTANTIATE_TEST_SUITE_P(functionTest, TestExiting, ::testing::Values("6hovno()", "skeropes_()", "else()", "float()", "function()", "if()", "int()", "null()", "return()", "string()", "void(),", "while()"));


        class TestComparing : public testing::TestWithParam<std::pair<std::string, std::vector<lexType>>> {
            // You can implement all the usual fixture class members here.
            // To access the test parameter, call GetParam() from class
            // TestWithParam<T>.
            FILE *fileForTest;

            void SetUp() override {
                fclose(fopen("test.txt", "w"));
                fileForTest = fopen("test.txt", "ab+");
                php = true;
                declare = true;
            }

            void TearDown() override {
                fclose(fileForTest);
                fclose(fopen("test.txt", "w"));
            }

        public:
            FILE *prepareFile(const char *text) {
                fprintf(fileForTest, "%s", text);
                rewind(fileForTest);
                fclose(fileForTest);
                fileForTest = fopen("test.txt", "r");
                return fileForTest;
            }
        };

        TEST_P(TestComparing, compareTest) {
            const char *string = std::get<0>(GetParam()).c_str();
            const std::vector<lexType> checkTokens = std::get<1>(GetParam());

            FILE *fp = this->prepareFile(string);

            for (auto i: checkTokens) {
                token_t t = getToken(fp);
                // EXPECT_STREQ(getTerminalName(t.type), getTerminalName(identifierVar));
                EXPECT_EQ((t.type), i);
                //EXPECT_EQ((t.type), (i));
            }
        }

        INSTANTIATE_TEST_SUITE_P(functionParamTest, TestComparing, ::testing::Values(std::make_pair(std::string("hovno(string $a)"), std::vector({
                                                                                                                                             identifierFunc,
                                                                                                                                             leftPar,
                                                                                                                                             stringKey,
                                                                                                                                             identifierVar,
                                                                                                                                             rightPar,
                                                                                                                                     })),
                                                                                     std::make_pair(std::string("hovno(int $a)"), std::vector({
                                                                                                                                          identifierFunc,
                                                                                                                                          leftPar,
                                                                                                                                           intKey,
                                                                                                                                          identifierVar,
                                                                                                                                          rightPar,
                                                                                                                                  })),
                                                                                     std::make_pair(std::string("hovno(float $a)"), std::vector({
                                                                                                                                            identifierFunc,
                                                                                                                                            leftPar,
                                                                                                                                            floatKey,
                                                                                                                                            identifierVar,
                                                                                                                                            rightPar,
                                                                                                                                    })),
                                                                                     std::make_pair(std::string("hovno(string? $a)"), std::vector({
                                                                                                                                              identifierFunc,
                                                                                                                                              leftPar,
                                                                                                                                              stringNullKey,
                                                                                                                                              identifierVar,
                                                                                                                                              rightPar,
                                                                                                                                      })),
                                                                                     std::make_pair(std::string("hovno(int? $a)"), std::vector({
                                                                                                                                           identifierFunc,
                                                                                                                                           leftPar,
                                                                                                                                           intNullKey,
                                                                                                                                           identifierVar,
                                                                                                                                           rightPar,
                                                                                                                                   })),
                                                                                     std::make_pair(std::string("hovno(float? $a)"), std::vector({
                                                                                                                                             identifierFunc,
                                                                                                                                             leftPar,
                                                                                                                                             floatNullKey,
                                                                                                                                             identifierVar,
                                                                                                                                             rightPar,
                                                                                                                                     })),
                                                                                     std::make_pair(std::string("hovno(string $a, float $a)"), std::vector({
                                                                                                                                                       identifierFunc,
                                                                                                                                                       leftPar,
                                                                                                                                                       floatNullKey,
                                                                                                                                                       identifierVar,
                                                                                                                                                       rightPar,
                                                                                                                                               })),
                                                                                     std::make_pair(std::string("hovno(string $a, int $a, float $a)"), std::vector({identifierFunc, leftPar, stringKey, identifierVar, floatKey, identifierVar, floatKey, identifierVar, rightPar}))));


        INSTANTIATE_TEST_SUITE_P(comments, TestComparing, ::testing::Values(std::make_pair(std::string("//skeropes"
                                                                                                       "int "
                                                                                                       "// int $a \n"
                                                                                                       "// */ \t \t                                              //hovno \n"
                                                                                                       "\t \t \t                                                "
                                                                                                       "/*hovnokod******************je///////////* super tvl*/ int "),
                                                                                           std::vector({intKey, intKey}))));

        INSTANTIATE_TEST_SUITE_P(variableMore2, TestComparing, ::testing::Values(
                                                                       std::make_pair(std::string("$hovno"), std::vector({identifierVar})),
                                                                       std::make_pair(std::string("$hPvnpS"), std::vector({identifierVar})),
                                                                       std::make_pair(std::string("$HOVNO"), std::vector({identifierVar})),
                                                                       std::make_pair(std::string("$HO999VNO"), std::vector({identifierVar})),
                                                                       std::make_pair(std::string("$hovno_kod"), std::vector({identifierVar})),
                                                                       std::make_pair(std::string("$hovnokod_"), std::vector({identifierVar}))
                                                                                 ));


        //region LexTestTokenData_stringLit
        TEST_F(LexTestTokenData, string_data) {
            std::string str = R"("Uplne jednoduchy ale delsi text")";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), str.substr(1,str.length()-2).c_str());
        }

        TEST_F(LexTestTokenData, string_data_long_newline_tabs) {
            std::string str = "\"Uplne jednoduchy ale delsi text \n\n\n\n\n\n\n\n\n\n\n\n\n ktery pokracuje na dalsi radek a jsou v nem i taby \"";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), str.substr(1,str.length()-2).c_str());
        }

        TEST_F(LexTestTokenData, string_long_tabs) {
            std::string str = "\"Uplne  ale delsi text ktery pokracuje na dalsi radek \t\t\t\t\t\t\t\t a jsou v nem i taby \t \"";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), str.substr(1,str.length()-2).c_str());
        }

        TEST_F(LexTestTokenData, string_verry_long) {
            std::string str = "\"Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby "
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\"";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), str.substr(1,str.length()-2).c_str());
        }

        TEST_F(LexTestTokenData, string_verry_long_newLine) {
            std::string str = "\""
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby \n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "\"";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), str.substr(1,str.length()-2).c_str());
        }

        TEST_F(LexTestTokenData, string_verry_long_newLine_comment) {
            std::string str = "\""
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby \n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n // commmmmm  \n "
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                              "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                              "\"";
            std::string rstr = "\""
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby \n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby\n"
                               "Uplne  ale delsi text ktery pokracuje na dalsi radek a jsou v nem i taby"
                               "\"";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), rstr.c_str());
        }

        TEST_F(LexTestTokenData, string_escape) {
            std::string str = R"("Ahoj\n\"Sve'te \\\034 \x00")";
            std::string rstr = "Ahoj\n" // ze zadání
                               "\"Sve'te \\\"";

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            EXPECT_STREQ(dstrGet(token.data.valueString), rstr.c_str());
        }
        // endregion


        TEST_F(LexTestTokenData, int_data) {
            std::vector<int> nums = {10, 20, 30, 500, 555, 99999, 4567, std::numeric_limits<int>::max()};
            std::string str;
            for (auto i: nums) {
                str += std::to_string(i);
                str += std::string("\n");
            }

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            for (auto i: nums)
                EXPECT_EQ(token.data.valueInteger, i);
        }

        TEST_F(LexTestTokenData, int_data_comments) {
            std::vector<int> nums = {10, 20, 30, 500, 555, 99999, 4567, std::numeric_limits<int>::max()};
            std::string str;
            for (auto i: nums) {
                str += std::to_string(i);
                str += std::string("//komentar \n");
                str += std::string("/*aji blokovy\n a tady\n taky\n a tam\n je nkonec_!*/");
            }

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            for (auto i: nums)
                EXPECT_EQ(token.data.valueInteger, i);
        }

        TEST_F(LexTestTokenData, float_data_exponencial) {
            FILE *fp = prepareFile("0.0e0 00000.15e0000 1.8498e0005 61561615.0000e15161561 ");
            std::vector<float> nums = {
                    0,
                    0.15,
                    1.8498e0 /*q*/,
                    005,
            };


            auto token = getToken(fp);

            for (auto i: nums)
                EXPECT_EQ(token.data.valueInteger, i);
        }

        TEST_F(LexTestTokenData, float_data_basic_comments) {
            std::vector<float> nums = {10, 20, 30, 500, 555, 99999, 4567, std::numeric_limits<float>::max()};
            std::string str;
            for (auto i: nums) {
                str += std::to_string(i);
                str += std::string("//komentar \n");
                str += std::string("/*aji blokovy\n a tady\n taky\n a tam\n je konec_!*/");
            }

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            for (auto i: nums)
                EXPECT_EQ(token.data.valueInteger, i);
        }

        TEST_F(LexTestTokenData, float_data_basic) {
            std::vector<float> nums = {10, 20, 30, 500, 555, 99999, 4567, std::numeric_limits<float>::max()};
            std::string str;
            for (auto i: nums) {
                str += std::to_string(i);
                str += std::string("//komentar \n");
                str += std::string("/*aji blokovy\n a tady\n taky\n a tam\n je nkonec_!*/");
            }

            FILE *fp = prepareFile(str.c_str());


            auto token = getToken(fp);

            for (auto i: nums)
                EXPECT_EQ(token.data.valueInteger, i);
        }


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
                          
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);;

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars2) {
            char text[] = "<?phpjenej\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars3) {
            char text[] = "<?php\n"
                          "a = 10;"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars4) {
            char text[] = "<<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars_comments1) {
            char text[] = "/*kks*/<<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars_comments2) {
            char text[] = "//kks\n<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars5) {
            char text[] = "\t<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars6) {
            char text[] = "\n<?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, prolog_unallowChars7) {
            char text[] = " <?php\n"
                          "declare(strict_types=1);"
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, function_declare_more_args) {

            char text[] = 
                          "function concat(string $x, string $y): string {"
                          "return $x . \" \" . $y;\n"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, stringKey, identifierVar,
                                           stringKey,
                                           identifierVar, rightPar, colon, stringKey, curlyBraceRight,
                                           returnKey, identifierVar, concatenationOp, stringLiteral,
                                           curlyBraceLeft};

            assertTokensEq(fp, tokens);
        }

        TEST_F(LexTestAdvanced, function_declare_empty_param) {

            char text[] = 
                          "function concat():string {"
                          "return \" \" \n;"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, rightPar, colon, stringKey,
                                           curlyBraceRight,
                                           returnKey, stringLiteral, semicolon,
                                           curlyBraceLeft};
            assertTokensEq(fp, tokens);
        }

        TEST_F(LexTestAdvanced, function_declare_name) {

            char text[] = 
                          "function 2concat():string {"
                          "return \" \" \n;"
                          "}";
            FILE *fp = prepareFile(text);

            getToken(fp);// function key
            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestAdvanced, function_declare_name_with_num) {

            char text[] = "function concat2():string {"
                          "return \" \" \n;"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, rightPar, colon, stringKey,
                                           curlyBraceRight,
                                           returnKey, stringLiteral, semicolon,
                                           curlyBraceLeft};

            assertTokensEq(fp, tokens);
        }

        TEST_F(LexTestAdvanced, function_declare) {

            char text[] = 
                          "function bar(string $param) : string {\n"
                          "return foo($param);\n"
                          "}";
            FILE *fp = prepareFile(text);

            std::vector<lexType> tokens = {functionKey, identifierFunc, leftPar, stringKey,
                                           identifierVar, rightPar, colon, stringKey, curlyBraceRight,
                                           returnKey, identifierFunc, identifierVar, semicolon, curlyBraceLeft};
            assertTokensEq(fp, tokens);
        }


        TEST_F(LexTestSimple, keywords_test) {
            FILE *fp = prepareFile("else float function if int null return"
                                   "string void while");


            assertTokensEq(fp, {elseKey, floatKey, functionKey, ifKey, intKey, nullKey,
                                returnKey, stringKey, voidKey, whileKey});
        }

        TEST_F(LexTestSimple, condition) {
            FILE *fp = prepareFile("if (1) { 1; } else { 7; }");


            assertTokensEq(fp, {ifKey, leftPar, integerLiteral, rightPar,
                                curlyBraceLeft, intKey, semicolon, curlyBraceRight,
                                elseKey, curlyBraceLeft, integerLiteral, semicolon, curlyBraceRight});

        }

        // literál je tvořen celou a desetinnou částí,
        TEST_F(LexTestSimple, floatTest) {
            FILE *fp = prepareFile("0.0 00000.0000 1.0005 61561615.15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }
        // nebo celou částí a exponen-tem,
        TEST_F(LexTestSimple, floatTestExponents1) {
            FILE *fp = prepareFile("00e0 000000e0000 10e0005 615616150e15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestSimple, floatTestExponents2) {
            FILE *fp = prepareFile("00e+0 000000e+0000 10e+0005 615616150e+15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestSimple, floatTestExponents3) {
            FILE *fp = prepareFile("00e-0 000000e-0000 10e-0005 615616150e-15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestSimple, floatTestExponents4) {
            FILE *fp = prepareFile("00E0 000000E0000 10E0005 615616150E15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestSimple, floatTestExponents5) {
            FILE *fp = prepareFile("00E+0 000000E+0000 10E+0005 615616150E+15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestSimple, floatTestExponents6) {
            FILE *fp = prepareFile("00E-0 000000E-0000 10E-0005 615616150E-15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }
        // nebo celou a desetinnou částí a exponentem.
        TEST_F(LexTestSimple, floatTestDecimalExponents1) {

            FILE *fp = prepareFile("0.0e0 00000.15e0000 1.8498e0005 61561615.0000e15161561 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestSimple, floatTestDecimalExponents2) {

            FILE *fp = prepareFile("0.0e0 0.0E0 0.0e-1 "
                                   "0.0e+1 0.0E+1 0.0E-1 ");


            assertTokensEq(fp, {
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestEdgeCase, floatFail1) {
            // desetinna cast nesmi byt pradna
            FILE *fp = prepareFile("0.e44");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail2) {
            // desetinna cast nesmi byt pradna
            FILE *fp = prepareFile("0.e44");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail3) {
            // cela cast nesmi byt pradna
            FILE *fp = prepareFile(".5e44");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail4) {
            // +- nelze
            FILE *fp = prepareFile("0.0e+-44");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }


        TEST_F(LexTestEdgeCase, floatFail5) {
            // -+ nelze
            FILE *fp = prepareFile("0.0e-+44");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail6) {
            // funguje nejvetsi mozne cislo
            auto text = std::to_string(INTMAX_MAX) + std::string(".69");
            FILE *fp = prepareFile(text.c_str());

            assertTokensEq(fp, {floatKey,});
        }

        TEST_F(LexTestEdgeCase, floatFail7) {
            // expo cast nesmí byt pradna
            FILE *fp = prepareFile("0.0e");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail8) {
            // expo cast nesmí byt pradna
            FILE *fp = prepareFile("0.0e+");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail9) {
            // expo cast nesmí byt pradna
            FILE *fp = prepareFile("0.0e-");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail10) {
            // expo cast nesmí byt pradna
            FILE *fp = prepareFile("0.0E");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail11) {
            // expo cast nesmí byt pradna
            FILE *fp = prepareFile("0.0E+");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, floatFail12) {
            // expo cast nesmí byt pradna
            FILE *fp = prepareFile("0.0E-");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestSimple, varTypeKeywords) {
            FILE *fp = prepareFile("float int string\n"
                                   "?float ?int ?string");


            assertTokensEq(fp, {floatKey, intKey, stringKey,
                                floatNullKey, intNullKey, stringNullKey});
        }

        TEST_F(LexTestSimple, functionParams) {
            FILE *fp = prepareFile("function bar(string $param) : string {\n");


            assertTokensEq(fp, {floatKey, intKey, stringKey,
                                floatNullKey, intNullKey, stringNullKey});
        }

        TEST_F(LexTestSimple, functionReturnValue1) {
            FILE *fp = prepareFile("function bar() : string {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, stringKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue2) {
            FILE *fp = prepareFile("function asfasflasflasdnflasdnf() : ?string {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, stringNullKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue3) {
            FILE *fp = prepareFile("function afdsf___ds() : int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, intKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue4) {
            FILE *fp = prepareFile("function bar() : ?int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, intNullKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue5) {
            FILE *fp = prepareFile("function bar() : float {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, floatKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionReturnValue6) {
            FILE *fp = prepareFile("function bar() : ?float {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, floatNullKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier0) {
            FILE *fp = prepareFile("function bar() : int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, intKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier1) {
            FILE *fp = prepareFile("function __4554() : int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, intKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier2) {
            FILE *fp = prepareFile("function _ASDF48() : int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, intKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, functionIdentifier3) {
            FILE *fp = prepareFile("function _ASDF48() : int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar, colon, intKey, curlyBraceLeft});
        }

        TEST_F(LexTestEdgeCase, functionIdentifier4) {
            // identifier cannot start with digits
            FILE *fp = prepareFile("function 48sadf_ds() : int {\n");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, functionIdentifier5) {
            // identifier cannot start with digits
            FILE *fp = prepareFile("function 6_6() : int {\n");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestSimple, functionIdentifier6) {
            FILE *fp = prepareFile("function AZaz__xy() : int {\n");


            assertTokensEq(fp, {functionKey, identifierFunc, leftPar, rightPar,
                                colon, intKey, curlyBraceLeft});
        }

        TEST_F(LexTestSimple, expressionsSimple0) {
            FILE *fp = prepareFile(R"("susenky" + 14)");


            assertTokensEq(fp, {stringLiteral, plusOp, integerLiteral});
        }

        TEST_F(LexTestSimple, expressionsSimple1) {
            FILE *fp = prepareFile(R"((15 + 36) * "susenky")");


            assertTokensEq(fp, {leftPar, integerLiteral, plusOp, integerLiteral, rightPar,
                                multiplicationOp, stringLiteral});
        }

        TEST_F(LexTestSimple, expressionsSimple2) {
            FILE *fp = prepareFile(R"((((14+69))))");


            assertTokensEq(fp, {
                                       leftPar,
                                       leftPar,
                                       leftPar,
                                       integerLiteral,
                                       plusOp,
                                       integerLiteral,
                                       rightPar,
                                       rightPar,
                                       rightPar,
                               });
        }

        TEST_F(LexTestSimple, expressionsSimple3) {
            FILE *fp = prepareFile(R"((((14+69))))");


            assertTokensEq(fp, {
                                       leftPar,
                                       leftPar,
                                       leftPar,
                                       integerLiteral,
                                       plusOp,
                                       integerLiteral,
                                       rightPar,
                                       rightPar,
                                       rightPar,
                               });
        }

        TEST_F(LexTestSimple, expressionsSimple4) {
            FILE *fp = prepareFile(R"(((("696996"/"696969"))))");


            assertTokensEq(fp, {
                                       leftPar,
                                       leftPar,
                                       leftPar,
                                       stringLiteral,
                                       divisionOp,
                                       stringLiteral,
                                       rightPar,
                                       rightPar,
                                       rightPar,
                               });
        }

        TEST_F(LexTestSimple, expressionsSimple5) {
            FILE *fp = prepareFile(R"(((("696996"/"696969"))))");


            assertTokensEq(fp, {
                                       leftPar,
                                       leftPar,
                                       leftPar,
                                       stringLiteral,
                                       divisionOp,
                                       stringLiteral,
                                       rightPar,
                                       rightPar,
                                       rightPar,
                               });
        }

        TEST_F(LexTestSimple, expressionsSimple6) {
            FILE *fp = prepareFile(R"(15.159e69*0.0e69+        6969699.698E99/)");


            assertTokensEq(fp, {floatLiteral, multiplicationOp,
                                floatLiteral, plusOp,
                                floatLiteral, divisionOp});
        }

        TEST_F(LexTestSimple, expressionsSimple7) {
            FILE *fp = prepareFile(R"("69696"* /* jak se mas */ / "6969" 
                                                            * "6969"/)");


            assertTokensEq(fp, {stringLiteral, multiplicationOp, divisionOp,
                                stringLiteral, multiplicationOp,
                                stringLiteral, divisionOp});
        }

        TEST_F(LexTestSimple, expressionsVariable4) {
            FILE *fp = prepareFile(R"(($_67/$cookie*$bruh+$____fuuuh-$huh)");


            assertTokensEq(fp, {identifierVar, divisionOp,
                                identifierVar, multiplicationOp,
                                identifierVar, plusOp,
                                identifierVar, minusOp});
        }

        TEST_F(LexTestSimple, expressionsVariable2) {
            FILE *fp = prepareFile(R"(($_67/$cookie*$bruh+$____fuuuh-$huh)");


            assertTokensEq(fp, {identifierVar, divisionOp,
                                identifierVar, multiplicationOp,
                                identifierVar, plusOp,
                                identifierVar, minusOp});
        }

        TEST_F(LexTestSimple, expressionsVariable3) {
            FILE *fp = prepareFile(R"(((($howyoudoin))))");


            assertTokensEq(fp, {
                                       leftPar,
                                       leftPar,
                                       leftPar,
                                       identifierVar,
                                       rightPar,
                                       rightPar,
                                       rightPar,
                               });
        }

        TEST_F(LexTestAdvanced, expressionsExtended) {
            FILE *fp = prepareFile(R"(($_69 + 69 /(0) * ($ABCD * $bruh)))");


            assertTokensEq(fp, {leftPar, identifierVar, plusOp, integerLiteral,
                                divisionOp, leftPar, integerLiteral, rightPar,
                                multiplicationOp, leftPar, identifierVar, multiplicationOp,
                                identifierVar, rightPar, rightPar});
        }

        TEST_F(LexTestAdvanced, expressionsExtended1) {
            FILE *fp = prepareFile(R"((("kokos")) + 67 * (($cookie)))");


            assertTokensEq(fp, {leftPar, leftPar, stringLiteral, rightPar, rightPar,
                                plusOp, integerLiteral, multiplicationOp,
                                leftPar, leftPar, identifierVar, rightPar, rightPar});
        }

        TEST_F(LexTestEdgeCase, weirdStringWithComments) {
            FILE *fp = prepareFile(R"("\"\"\"\"\"susenky\"\"\"\"\multiplicationOp"" / /*Huh how r u**// ""\"huhu\"\"")");


            assertTokensEq(fp, {stringLiteral, divisionOp, divisionOp, stringLiteral});
        }
        TEST_F(LexTestEdgeCase, expression) {
            // Two operands in exp
            FILE *fp = prepareFile("/*");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression1) {
            // Two operands in exp
            FILE *fp = prepareFile("/+");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression2) {
            // More operands in exp
            FILE *fp = prepareFile("+/*-");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression3) {
            FILE *fp = prepareFile(R"($cookie + 998 / "bruhie" *-)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, expression4) {
            FILE *fp = prepareFile(R"($cookie * $bruh/// This should not return divisionOp)");


            assertTokensEq(fp, {identifierVar, multiplicationOp, identifierVar});
        }

        TEST_F(LexTestEdgeCase, expression5) {
            FILE *fp = prepareFile(R"(/* this is comment*/ / / /)");


            assertTokensEq(fp, {
                                       divisionOp,
                                       divisionOp,
                                       divisionOp,
                               });
        }

        TEST_F(LexTestSimple, prologAtTheEnd1) {
            FILE *fp = prepareFile(R"(1 ?>)");


            assertTokensEq(fp, {integerLiteral});
        }

        TEST_F(LexTestSimple, prologAtTheEnd2) {
            FILE *fp = prepareFile(R"(1 ?>)");


            assertTokensEq(fp, {integerLiteral});
        }

        TEST_F(LexTestSimple, prologAtTheEnd3) {
            FILE *fp = prepareFile(R"(1 ?>)");


            assertTokensEq(fp, {integerLiteral});
        }

        TEST_F(LexTestSimple, prologAtTheEnd4) {
            FILE *fp = prepareFile(R"(//msg @Fado if you read this \n 1 ?>)");


            assertTokensEq(fp, {integerLiteral});
        }

        TEST_F(LexTestEdgeCase, prologAtTheEnd5) {
            FILE *fp = prepareFile(R"(//how much wood would a woodchuck chuck \n\n\n ?>)");


            assertTokensEq(fp, {ending});
        }

        TEST_F(LexTestSimple, prologAtTheEnd6) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?>)");


            assertTokensEq(fp, {intNullKey, floatNullKey, stringNullKey, ending});
        }

        TEST_F(LexTestSimple, prologAtTheEndIsNotEnd) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?>)");


            assertTokensEq(fp, {intNullKey, floatNullKey, stringNullKey, ending});
        }

        TEST_F(LexTestSimple, prologAtTheEndIsNotEnd1) {
            FILE *fp = prepareFile(R"("kok" 45 56.5 ?>)");


            assertTokensEq(fp, {
                                       stringLiteral,
                                       integerLiteral,
                                       floatLiteral,
                               });
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?> ?int ?float ?string)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd1) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?> ?int ?float ?string)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd2) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?>>)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd3) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?> /* not even comment is legit here*/ )");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd4) {
            FILE *fp = prepareFile(R"(//how dare you ?> ">>>> hehe <<<<<")");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd5) {
            FILE *fp = prepareFile(R"(?int ?float ?string ?> ?>)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd6) {
            FILE *fp = prepareFile(R"(int float $nothing_interesting ?>\t)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd7) {
            FILE *fp = prepareFile(R"(int float $nothing_interesting ?>\n)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, prologAtTheEndIsNotEnd8) {
            // not even white spaces after end
            FILE *fp = prepareFile(R"(int float $nothing_interesting ?> )");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, whiteSpaces1) {
            FILE *fp = prepareFile(R"(\t\t\n65\n67\t67)");


            assertTokensEq(fp, {integerLiteral, integerLiteral, integerLiteral});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces2) {
            FILE *fp = prepareFile(R"(\nn)");


            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces3) {
            FILE *fp = prepareFile(R"(\nn)");


            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces4) {
            FILE *fp = prepareFile(R"(\tn)");


            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces5) {
            FILE *fp = prepareFile(R"( n)");


            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces6) {
            FILE *fp = prepareFile(R"(\n\t    \t\n\n\n huhu)");


            assertTokensEq(fp, {identifierFunc, ending});
        }

        TEST_F(LexTestEdgeCase, whiteSpaces7) {
            FILE *fp = prepareFile(R"(\n\t    \t\n\n\n  )");


            assertTokensEq(fp, { ending });
        }

        TEST_F(LexTestEdgeCase, whiteSpaces8) {
            FILE *fp = prepareFile(R"(\nn\tt t)");


            assertTokensEq(fp, { identifierFunc, identifierFunc, identifierFunc, });
        }

        TEST_F(LexTestEdgeCase, whiteSpaces9) {
            FILE *fp = prepareFile(R"(\   n)");


            EXPECT_EXIT(getToken(fp);, ::testing::ExitedWithCode(ERR_LEX), ".*");
        }

        TEST_F(LexTestEdgeCase, whiteSpaces10) {
            FILE *fp = prepareFile(R"("\n\t\t\n  \t\n"?>)");


            assertTokensEq(fp, { stringLiteral });
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces1) {
            FILE *fp = prepareFile(R"(75 -/*kkt*/66)");


            assertTokensEq(fp, { integerLiteral, minusOp, integerLiteral });
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces2) {
            FILE *fp = prepareFile(R"(;}$kks="huh";)");


            assertTokensEq(fp, { semicolon, curlyBraceRight, identifierVar,
                                 stringLiteral, stringLiteral});
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces3) {
            FILE *fp = prepareFile(R"(();6.6e9())");


            assertTokensEq(fp, { leftPar, rightPar,
                                 semicolon, floatLiteral,
                                 leftPar, rightPar,});
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces4) {
            FILE *fp = prepareFile(R"($huh=$bruh*{}-;)");


            assertTokensEq(fp, { identifierVar, eqOp,identifierVar,
                                 multiplicationOp, curlyBraceLeft, curlyBraceRight,
                                 minusOp, semicolon});
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces5) {
            FILE *fp = prepareFile(R"($kks=0;$kks-$kks;)");


            assertTokensEq(fp, { identifierVar, eqOp, integerLiteral, semicolon,
                                 identifierVar, minusOp, identifierVar, semicolon});
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces6) {
            FILE *fp = prepareFile(R"(return"sracka";)");


            assertTokensEq(fp, { returnKey, stringLiteral, semicolon});
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces7) {
            FILE *fp = prepareFile(R"(return($kks);)");


            assertTokensEq(fp, { returnKey, leftPar, identifierVar, rightPar, semicolon});
        }

        TEST_F(LexTestEdgeCase, noWhiteSpaces8) {
            FILE *fp = prepareFile(R"(return$bruh;)");


            assertTokensEq(fp, { returnKey,  identifierVar, semicolon});
        }

        TEST_F(LexTestEdgeCase, oneStateChars) {
            // these chars should yield to token immediately
            FILE *fp = prepareFile(R"({};()+-*/=<>:,)");


            assertTokensEq(fp, { curlyBraceLeft, curlyBraceRight, semicolon,
                                 leftPar, rightPar, plusOp, minusOp, multiplicationOp,
                                 divisionOp, eqOp, lesserThanOp, greaterThanOp,
                                 colon, comma, });
        }

        TEST_F(LexTestEdgeCase, oneStateChars1) {
            // these chars should yield to token immediately
            FILE *fp = prepareFile(R"({};()+-*/=<>:,)");


            assertTokensEq(fp, { curlyBraceLeft, curlyBraceRight, semicolon,
                                 leftPar, rightPar, plusOp, minusOp, multiplicationOp,
                                 divisionOp, eqOp, lesserThanOp, greaterThanOp,
                                 colon, comma, });
        }



        // Asi by to chtelo slozite funkce
    }// namespace stack
}// namespace ifj22
