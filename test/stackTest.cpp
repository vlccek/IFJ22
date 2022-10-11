

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
            genericStack *st = gStackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                gStackPush(st, &array[i]);
            }

            for (int i = std::size(array) - 1; i > 0; i--) {
                int *out = (int *) gStackPop(st);
                ASSERT_EQ(array[i], *out);
            }
        }

        TEST(stackTest, empty_after_init) {
            genericStack *stack = gStackInit();

            ASSERT_TRUE(gStackIsEmpty(stack));
        }

        TEST(stackTest, not_empty_after_push) {
            genericStack *stack = gStackInit();
            int x = 69;
            gStackPush(stack, &x);

            ASSERT_FALSE(gStackIsEmpty(stack));
        }

        TEST(stackTest, empty_push_pop) {
            genericStack *stack = gStackInit();
            int x = 69;
            gStackPush(stack, &x);

            ASSERT_FALSE(gStackIsEmpty(stack));
            int y = *(int *) gStackPop(stack);
            ASSERT_TRUE(gStackIsEmpty(stack));
        }

        TEST(stackTest, int_push_pop_eq) {
            genericStack *stack = gStackInit();
            int x = 69;
            gStackPush(stack, &x);
            int y = *(int *) gStackPop(stack);

            ASSERT_EQ(x, y);
        }

        TEST(stackTest, value_in_pointer_not_changing_after_push) {
            genericStack *stack = gStackInit();
            int array_not_push[] = {69, 70, 71};
            int array[] = {69, 70, 71};
            for (int i = 0; i < std::size(array); i++) {
                gStackPush(stack, &array[i]);
            }

            for (int i = 0; i < std::size(array); i++) {
                ASSERT_EQ(array[i], array_not_push[i]);
            }
        }

        TEST(stackTest, pop_on_empty_stack) {
            genericStack *stack = gStackInit();
            // Pop on empty should not crash with sigsegv
            // Instead print error and exit with (I guess) internal error code
            // 'You should use function emptyStack()' error
            ASSERT_EXIT(gStackPop(stack), ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");
            ASSERT_EXIT(gStackPopBack(stack), ::testing::ExitedWithCode(IE_pop_empty_stack), ".*");
        }

        TEST(stackTest, push_stack_to_stack) {
            genericStack *inner_stack = gStackInit();
            genericStack *stack = gStackInit();
            int a = 1;
            int b = 2;

            gStackPush(inner_stack, &a);
            gStackPush(inner_stack, &b);
            gStackPush(stack, inner_stack);

            int *x = (int *) gStackPop(inner_stack);
            int *y = (int *) gStackPop(inner_stack);

            ASSERT_EQ(b, *x);
            ASSERT_EQ(a, *y);

            ASSERT_TRUE(gStackIsEmpty(inner_stack));
            ASSERT_FALSE(gStackIsEmpty(stack));
        }

        TEST(stackTest, stack_name_not_stack_type) {
            ASSERT_TRUE(true);
            // unfortunately reflection in C is not possible
        }

        TEST(stackTest, pop_back_empty) {
            genericStack *stack = gStackInit();
            int x = 1;
            gStackPush(stack, &x);
            void *y = gStackPopBack(stack);

            ASSERT_TRUE(gStackIsEmpty(stack));
        }

        TEST(stackTest, pop_back_not_empty) {
            genericStack *stack = gStackInit();
            int x = 1;
            gStackPush(stack, &x);
            gStackPush(stack, &x);
            void *y = gStackPopBack(stack);

            ASSERT_FALSE(gStackIsEmpty(stack));
        }

        TEST(stackTest, pop_back_int) {
            genericStack *stack = gStackInit();
            int x = 69;

            gStackPush(stack, &x);
            int y = *(int *) gStackPop(stack);

            ASSERT_EQ(x, y);
        }

        TEST(stackTest, pop_back_many) {
            genericStack *stack = gStackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                gStackPush(stack, &array[i]);
            }

            for (int i = 0; i < std::size(array); i++) {
                int *out = (int *) gStackPopBack(stack);
                ASSERT_EQ(array[i], *out);
            }
        }

        TEST(stackTest, pop_back_many_empty) {
            genericStack *stack = gStackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                gStackPush(stack, &array[i]);
            }

            for (int i = 0; i < std::size(array); i++) {
                gStackPopBack(stack);
            }
            ASSERT_TRUE(gStackIsEmpty(stack));
        }
        TEST(stackTest, getElement) {
            genericStack *s = gStackInit();
            int a = 10;
            int b = 11;
            int c = 12;
            int d = 13;
            int e = 13;


            gStackPush(s, &a);
            gStackPush(s, &b);
            gStackPush(s, &c);
            gStackPush(s, &d);
            gStackPush(s, &e);

            int *x = static_cast<int *>(gStackGetNth(s, 0));
            int *z = static_cast<int *>(gStackGetNth(s, 2));
            int *y = static_cast<int *>(gStackGetNth(s, 15));

            ASSERT_EQ(13, *x);
            ASSERT_EQ(12, *z);
            ASSERT_EQ(NULL, y);
        }


        TEST(stackTest, getElementMore) {
            genericStack *stack = gStackInit();

            int array[] = {10, 20, 30, 40, 50};

            for (int i = 0; i < std::size(array); i++) {
                gStackPush(stack, &array[i]);
            }
            int l = std::size(array);
            for (int i = 0; i < std::size(array); i++) {
                ASSERT_EQ(*static_cast<int *>(gStackGetNth(stack, i)), array[--l]);
            }
        }
    }// namespace stack
}// namespace ifj22
