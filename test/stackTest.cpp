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

        TEST(stackTest, empty_after_init) {
            stack_type *stack = stackInit();

            ASSERT_TRUE(sIsEmpty(stack));
        }

        TEST(stackTest, not_empty_after_push) {
            stack_type *stack = stackInit();
            int x = 69;
            push(stack, &x);

            ASSERT_FALSE(sIsEmpty(stack));
        }

        TEST(stackTest, empty_push_pop) {
            stack_type *stack = stackInit();
            int x = 69;
            push(stack, &x);

            ASSERT_FALSE(sIsEmpty(stack));
            int y = *(int *) pop(stack);
            ASSERT_TRUE(sIsEmpty(stack));
        }

        TEST(stackTest, int_push_pop_eq) {
            stack_type *stack = stackInit();
            int x = 69;
            push(stack, &x);
            int y = *(int *) pop(stack);

            ASSERT_EQ(x, y);
        }

        TEST(stackTest, value_in_pointer_not_changing_after_push) {
            stack_type *stack = stackInit();
            int array_not_push[] = {69, 70, 71};
            int array[] = {69, 70, 71};
            for (int i = 0; i < std::size(array); i++) {
                push(stack, &array[i]);
            }

            for (int i = 0; i < std::size(array); i++) {
                ASSERT_EQ(array[i], array_not_push[i]);
            }
        }

        TEST(stackTest, pop_on_empty_stack) {
            stack_type *stack = stackInit();
            // Pop on empty should not crash with sigsegv
            // Instead print error and exit with (I guess) internal error code
            // 'You should use function emptyStack()' error
            ASSERT_EXIT((pop(stack), exit(0)), ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");
            ASSERT_EXIT((popBack(stack), exit(0)), ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");
        }

        TEST(stackTest, push_stack_to_stack) {
            stack_type *inner_stack = stackInit();
            stack_type *stack = stackInit();
            int a = 1;
            int b = 2;

            push(inner_stack, &a);
            push(inner_stack, &b);
            push(stack, inner_stack);

            int *x = (int *) pop(inner_stack);
            int *y = (int *) pop(inner_stack);

            ASSERT_EQ(b, *x);
            ASSERT_EQ(a, *y);

            ASSERT_TRUE(sIsEmpty(inner_stack));
            ASSERT_FALSE(sIsEmpty(stack));
        }

        TEST(stackTest, stack_name_not_stack_type) {
            ASSERT_TRUE(false);
            // unfortunately reflection in C is not possible
        }

        TEST(stackTest, pop_back_empty) {
            stack_type *stack = stackInit();
            int x = 1;
            push(stack, &x);
            void *y = popBack(stack);

            ASSERT_TRUE(sIsEmpty(stack));
        }

        TEST(stackTest, pop_back_not_empty) {
            stack_type *stack = stackInit();
            int x = 1;
            push(stack, &x);
            push(stack, &x);
            void *y = popBack(stack);

            ASSERT_FALSE(sIsEmpty(stack));
        }

        TEST(stackTest, pop_back_int) {
            stack_type *stack = stackInit();
            int x = 69;

            push(stack, &x);
            int y = *(int *) pop(stack);

            ASSERT_EQ(x, y);
        }

        TEST(stackTest, pop_back_many) {
            stack_type *stack = stackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                push(stack, &array[i]);
            }

            for (int i = 0; i < std::size(array); i++) {
                int *out = (int *) popBack(stack);
                ASSERT_EQ(array[i], *out);
            }
        }

        TEST(stackTest, pop_back_many_empty) {
            stack_type *stack = stackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                push(stack, &array[i]);
            }

            for (int i = 0; i < std::size(array); i++) {
                popBack(stack);
            }
            ASSERT_TRUE(sIsEmpty(stack));
        }

    }
}
