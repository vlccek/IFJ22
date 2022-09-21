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

        TEST(stackTest, empty_after_init){
            stack_type *stack = stackInit();

            ASSERT_TRUE(sIsEmpty(stack));
        }

        TEST(stackTest, not_empty_after_push){
            stack_type *stack = stackInit();
            int x = 69;
            push(stack, &x);

            ASSERT_FALSE(sIsEmpty(stack));
        }

        TEST(stackTest, empty_push_pop){
            stack_type *stack = stackInit();
            int x = 69;
            push(stack, &x);

            ASSERT_FALSE(sIsEmpty(stack));
            int y = *(int*)pop(stack);
            ASSERT_TRUE(sIsEmpty(stack));
        }

        TEST(stackTest, int_push_pop_eq){
            stack_type *stack = stackInit();
            int x = 69;
            push(stack, &x);
            int y = *(int*)pop(stack);

            ASSERT_EQ(x, y);
        }

        TEST(stackTest, pointer_not_changing_after_push){

        }
    }
}
