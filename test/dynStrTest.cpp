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
            char *tmp = "Tvojem mamam!";

            dstrAppend(dstr, tmp);

            char *a = dstrGet(dstr);

            for (int i = 0; i < strlen(tmp); i++) {
                ASSERT_EQ(tmp[i], a[i]);
            }


        }
    }// namespace stack
}// namespace ifj22
