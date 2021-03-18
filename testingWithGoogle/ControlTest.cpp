#include "Control.hpp"
#include "Exception.hpp"
#include "Valuemap.hpp"
#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <stdexcept>
#include <nlohmann/json.hpp>

TEST(Control, Operator) {

  Eigen::Vector2d a(0.0, 1.0);
  Eigen::Vector2d b(1.0, 2.0);
  Eigen::Vector2d c(2.0, 3.0);

  nlohmann::json control_value_map;

  control_value_map = {{"id", "N213"},
                        {"data",
                         {{{"time", 1.0}, {"values", {a(0), a(1)}}},
                          {{"time", 2.0}, {"values", {b(0), b(1)}}},
                          {{"time", 3.0}, {"values", {c(0), c(1)}}}}}};


  Model::Networkproblem::Control<2>
      control_object(control_value_map);

  EXPECT_THROW(control_object(3.5),std::runtime_error);
  EXPECT_THROW(control_object(0.5), std::runtime_error);
  // EXPECT_ANY_THROW(boundary_object(0.5));

  // TEST2
  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = control_object(1.0);
    v2 = a;
    // ACT
    EXPECT_EQ(v, v2);
  }

  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = control_object(3.0);
    v2 = c;
    // ACT
    EXPECT_EQ(v, v2);
  }

  // ASSERT
  // Test that returns value of interpolation for specific time step
  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = control_object(2.5);
    v2 = 0.5 * (b + c);
    // ACT
    EXPECT_EQ(v, v2);
  }
}
