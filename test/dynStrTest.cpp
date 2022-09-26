#include "gtest/gtest.h"

// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../dynstring.h"

#include <stdio.h>
};

namespace ifj22 {
    namespace stack {

        class dstrTest : public ::testing::Test {
            dstrTest() {}
        };

        TEST(dstrTest, basicTest) {
            dynStr_t *dstr = dstrInit();
            char tmp[] = "Hello world!";
            dstrAppend(dstr, tmp);
            char *a = dstrGet(dstr);
            ASSERT_STREQ(tmp, a);
        }

        TEST(dstrTest, allCharTest)
        {
            dynStr_t *dstr = dstrInit();
            char all[256];
            // loop 1 to 255 adding each char to char array
            for (int i = 1; i < 256; i++) {
                all[i - 1] = i;
            }
            //terminate the string
            all[255] = '\0';
            dstrPrepend(dstr, all);
            char *a = dstrGet(dstr);
            ASSERT_STREQ(all, a);

        }

        TEST(dstrTest, appendTest) {
            dynStr_t *dstr = dstrInit();
            char tmp[] = "Tvojem mamam!";

            dstrAppend(dstr, tmp);
            dstrAppend(dstr, tmp);

            char *a = dstrGet(dstr);
            ASSERT_STREQ("Tvojem mamam!Tvojem mamam!", a);
        }
        // test for dstrPrepend
        TEST(dstrTest, prependTest) {
            dynStr_t *dstr = dstrInit();
            char tmp[] = "Tvojem mamam!";

            dstrPrepend(dstr, tmp);
            dstrPrepend(dstr, tmp);

            char *a = dstrGet(dstr);
            ASSERT_STREQ("Tvojem mamam!Tvojem mamam!", a);
        }

        TEST(dstrTest, append_compare_test) {
            dynStr_t *test = dstrInit();
            dynStr_t *to_compare = dstrInit();

            dstrAppend(test, "test");
            dstrAppend(test, " test");

            dstrAppend(to_compare, "test test");
            ASSERT_STREQ(dstrGet(test), dstrGet(to_compare));
            ASSERT_TRUE(dstrCmp(test, to_compare));
        }

        TEST(dstrTest, prepend_compare_test) {
            dynStr_t *test = dstrInit();
            dynStr_t *to_compare = dstrInit();

            dstrPrepend(test, "test");
            dstrPrepend(test, "test ");

            dstrPrepend(to_compare, "test test");
            ASSERT_STREQ(dstrGet(test), dstrGet(to_compare));
            ASSERT_TRUE(dstrCmp(test, to_compare));
        }

        TEST(dstrTest, append_prepend_compare_test) {
            dynStr_t *test = dstrInit();
            dynStr_t *to_compare = dstrInit();

            dstrAppend(test, "test");
            dstrPrepend(test, "test ");
            dstrAppend(test, "test");


            dstrPrepend(to_compare, "test testtest");
            ASSERT_STREQ(dstrGet(test), dstrGet(to_compare));
            ASSERT_TRUE(dstrCmp(test, to_compare));
        }

        // test dstrInitChar
        TEST(dstrTest, initCharTest) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            char *a = dstrGet(dstr);
            ASSERT_STREQ(tmp, a);
        }

        // test dstrLen
        TEST(dstrTest, lenTest) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            ASSERT_EQ(strlen(tmp), dstrLen(dstr));
        }

        // test dstrSubstring
        TEST(dstrTest, substringTest) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 0, 5);
            char *a = dstrGet(sub);
            ASSERT_STREQ("Tvoje", a);
        }

        // test dstrSubstring
        TEST(dstrTest, substringTest2) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 0, 0);
            char *a = dstrGet(sub);
            ASSERT_STREQ("", a);
        }

        // test dstrSubstring
        TEST(dstrTest, substringTest3) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 0, 100);
            char *a = dstrGet(sub);
            ASSERT_STREQ(tmp, a);
        }

        // test dstrSubstring
        TEST(dstrTest, substringTest4) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 7, 100);
            char *a = dstrGet(sub);
            ASSERT_STREQ("mamam!", a);
        }

        // test dstrSubstring with negative start
        TEST(dstrTest, substringTest5) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, -1, 100);
            char *a = dstrGet(sub);
            ASSERT_STREQ("!", a);
        }

        // test dstrSubstring with negative end
        TEST(dstrTest, substringTest6) {
            char tmp[] = "Tvojem mamam!";
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 0, -1);
            char *a = dstrGet(sub);
            ASSERT_STREQ("Tvojem mamam", a);
        }

        //test dstrInitChar with NULL
        TEST(dstrTest, initCharTest2) {
            dynStr_t *dstr = dstrInitChar(NULL);
            char *a = dstrGet(dstr);
            ASSERT_STREQ("", a);
        }

        //test dstrcharinit with string longer than 1024
        TEST(dstrTest, initCharTest3) {
            char tmp[1025];
            for (int i = 0; i < 1025; i++) {
                tmp[i] = 'a';
            }
            dynStr_t *dstr = dstrInitChar(tmp);
            char *a = dstrGet(dstr);
            ASSERT_STREQ(tmp, a);
        }

        // test dstrSubstring with string longer than 1024
        TEST(dstrTest, substringTest7) {
            char tmp[1025];
            for (int i = 0; i < 1024; i++) {
                tmp[i] = 'a';
            }
            tmp[1024] = '\0';
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 0, 1024);
            char *a = dstrGet(sub);
            ASSERT_STREQ(tmp, a);
        }

        // test dstrSubstring with string longer than 1000000
        TEST(dstrTest, substringTest8) {
            char tmp[1000001];
            for (int i = 0; i < 1000000; i++) {
                tmp[i] = 'a';
            }
            tmp[999999] = 'b';
            tmp[1000000] = '\0';
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, -1, 0);
            char *a = dstrGet(sub);
            ASSERT_STREQ("b", a);
        }
        // test dstrSubstring with string longer than 1000000
        TEST(dstrTest, substringTest9) {
            char tmp[1000001];
            for (int i = 0; i < 1000000; i++) {
                tmp[i] = 'a';
            }
            tmp[1000000] = '\0';
            dynStr_t *dstr = dstrInitChar(tmp);
            dynStr_t *sub = dstrSubstring(dstr, 0, 1000001);
            char *a = dstrGet(sub);
            ASSERT_STREQ(tmp, a);
        }



}// namespace stack
}// namespace ifj22
