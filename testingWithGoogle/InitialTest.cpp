#include "Initialvalue.hpp"
#include "Exception.hpp"
#include "Valuemap.hpp"
#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <stdexcept>
#include <nlohmann/json.hpp>

TEST(Initialvalue, Operator) {

  Eigen::Vector2d a;
  a(0) = 0.0;
  a(1) = 1.0;

  Eigen::Vector2d b;
  b(0) = 1.0;
  b(1) = 2.0;

  Eigen::Vector2d c;
  c(0) = 2.0;
  c(1) = 3.0;
 
  // std::map<double, Eigen::Vector2d> initial_value_map;
  // initial_value_map.insert(std::make_pair(1.0, a));
  // initial_value_map.insert(std::make_pair(2.0, b));
  // initial_value_map.insert(std::make_pair(3.0, c));

  nlohmann::json initial_value_map;

  initial_value_map = {
    {"id" , "N213"},
    {"data" , {
        {{"x" , 1.0}, {"values" , { 0.0, 1.0 }}},
        {{"x" , 2.0}, {"values" , { 1.0, 2.0 }}},
        {{"x" , 3.0}, {"values" , { 2.0, 3.0 }}}
      }
    }
  };

  Model::Networkproblem::Initialvalue<2>
      initial_object(initial_value_map);

  EXPECT_THROW(initial_object(3.5),Exception);
  EXPECT_THROW(initial_object(0.5), Exception);
  // EXPECT_ANY_THROW(initial_object(0.5));

  // TEST2
  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = initial_object(1.0);
    v2 = a;
    // ACT
    EXPECT_EQ(v, v2);
  }

  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = initial_object(3.0);
    v2 = c;
    // ACT
    EXPECT_EQ(v, v2);
  }

  // ASSERT
  // Test that returns value of interpolation for specific time step
  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = initial_object(2.5);
    v2 = 0.5 * (b + c);
    // ACT
    EXPECT_EQ(v, v2);
  }
}