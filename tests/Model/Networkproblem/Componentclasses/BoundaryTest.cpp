#include "Boundaryvalue.hpp"
#include "Exception.hpp"
#include "Valuemap.hpp"
#include <Eigen/Dense>
#include <exception>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(Boundaryvalue, Operator) {

  Eigen::Vector2d a(0.0, 1.0);
  Eigen::Vector2d b(1.0, 2.0);
  Eigen::Vector2d c(2.0, 3.0);

  nlohmann::json boundary_value_map;

  boundary_value_map
      = {{"id", "N213"},
         {"data",
          {{{"time", 1.0}, {"values", {a(0), a(1)}}},
           {{"time", 2.0}, {"values", {b(0), b(1)}}},
           {{"time", 3.0}, {"values", {c(0), c(1)}}}}}};

  Model::Networkproblem::Boundaryvalue<2> boundary_object(boundary_value_map);

  try {
    boundary_object(3.5);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("Out-of-range error in boundary values"));
  }

  try {
    boundary_object(0.5);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("Out-of-range error in boundary values"));
  }

  // TEST2
  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = boundary_object(1.0);
    v2 = a;
    // ACT
    EXPECT_EQ(v, v2);
  }

  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = boundary_object(3.0);
    v2 = c;
    // ACT
    EXPECT_EQ(v, v2);
  }

  // ASSERT
  // Test that returns value of interpolation for specific time step
  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = boundary_object(2.5);
    v2 = 0.5 * (b + c);
    // ACT
    EXPECT_EQ(v, v2);
  }
}
