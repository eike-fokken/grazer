#include "Normaldistribution.hpp"
#include <gtest/gtest.h>
#include <map>
#include <string>

TEST(Normaldistribution, operator1) {

  // Careful: this is a probabilistic test and might fail from time to time...

  int number_of_samples = 1e5;
  Aux::Normaldistribution n;

  double mean = 599.0;
  double standard_deviation = 100.0;

  std::map<int, int> occurcences;
  for (int number = 0; number < number_of_samples; ++number) {
    auto current = n(mean, standard_deviation);
    int index
        = static_cast<int>(std::floor((current - mean) / standard_deviation));
    ++occurcences[index];
  }

  auto share_0_plus = occurcences[0] / static_cast<double>(number_of_samples);
  auto share_1_plus = occurcences[1] / static_cast<double>(number_of_samples);
  auto share_2_plus = occurcences[2] / static_cast<double>(number_of_samples);

  auto share_0_minus = occurcences[-1] / static_cast<double>(number_of_samples);
  auto share_1_minus = occurcences[-2] / static_cast<double>(number_of_samples);
  auto share_2_minus = occurcences[-3] / static_cast<double>(number_of_samples);

  // Normal distribution:
  // 1 sigma: 68.27%
  // 2 sigma: 95.45%
  // 3 sigma: 99.73%

  EXPECT_LE(share_0_plus, 0.35);
  EXPECT_GE(share_0_plus, 0.335);

  EXPECT_LE(share_1_plus, 0.14);
  EXPECT_GE(share_1_plus, 0.13);

  EXPECT_LE(share_2_plus, 0.025);
  EXPECT_GE(share_2_plus, 0.019);

  EXPECT_LE(share_0_minus, 0.35);
  EXPECT_GE(share_0_minus, 0.335);

  EXPECT_LE(share_1_minus, 0.14);
  EXPECT_GE(share_1_minus, 0.13);

  EXPECT_LE(share_2_minus, 0.025);
  EXPECT_GE(share_2_minus, 0.019);
}
