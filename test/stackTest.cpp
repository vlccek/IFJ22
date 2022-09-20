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

TEST(stackTest, randomtest) {
  ASSERT_EQ(10, 10);
}
}
}