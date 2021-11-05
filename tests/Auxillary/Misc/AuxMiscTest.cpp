#include "Misc.hpp"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

TEST_OVERFLOW() {
  int a = 2147483647; // 2**31 - 1
  int b = 0;

  // Test that no overflow occurs
  EXPECT_EQ(Aux::safe_addition(a, b), 2147483647);

  // Test that overflow occurs and exception is thrown
  try {
    int c = Aux::safe_addition(a, b + 1);
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("The addition would cause an overflow."));
  }
}
