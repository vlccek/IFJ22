#include "gtest/gtest.h"

// Hide main
#define TESTING
// Hide the io function since this will segfault in testing
extern "C" {
#include "../stack.h"

#include <stdio.h>

};

namespace ifj22 {
    namespace stack {

        class stackTest : public ::testing::Test {
            stackTest() {}
        };

        TEST(stackTest, poptest) {
            stack_type *st = stackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                push(st, &array[i]);
            }

            for (int i = std::size(array) - 1; i > 0; i--) {
                int *out = (int *) pop(st);
                ASSERT_EQ(array[i], *out);
            }
        }
    }
}
