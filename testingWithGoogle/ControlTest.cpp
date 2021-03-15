#include "Control.hpp"
#include "Exception.hpp"
#include "Valuemap.hpp"
#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <stdexcept>
#include <nlohmann/json.hpp>

TEST(Control, Operator) {

  Eigen::Vector2d a;
  a(0) = 0.0;
  a(1) = 1.0;

  Eigen::Vector2d b;
  b(0) = 1.0;
  b(1) = 2.0;

  Eigen::Vector2d c;
  c(0) = 2.0;
  c(1) = 3.0;

  // std::map<double, Eigen::Vector2d> boundary_value_map;
  // boundary_value_map.insert(std::make_pair(1.0, a));
  // boundary_value_map.insert(std::make_pair(2.0, b));
  // boundary_value_map.insert(std::make_pair(3.0, c));

  nlohmann::json control_value_map;

  control_value_map = {
    {"id" , "N213"},
    {"data" , {
        {{"time" , 1.0}, {"values" , { 0.0, 1.0 }}},
        {{"time" , 2.0}, {"values" , { 1.0, 2.0 }}},
        {{"time" , 3.0}, {"values" , { 2.0, 3.0 }}}
      }
    }
  };

  Model::Networkproblem::Control<double, 2>
      control_object(control_value_map);

  EXPECT_THROW(control_object(3.5),Exception);
  EXPECT_THROW(control_object(0.5), Exception);
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
