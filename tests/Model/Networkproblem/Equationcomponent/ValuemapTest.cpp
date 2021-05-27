#include "Valuemap.hpp"
#include "Exception.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/src/Core/Matrix.h>
#include <exception>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

TEST(OperatorTEST, interpolation) {

  Eigen::Vector2d a(0.0, 1.0);
  Eigen::Vector2d b(1.0, 2.0);
  Eigen::Vector2d c(2.0, 3.0);

  nlohmann::json values_json;

  values_json
      = {{"id", "N213"},
         {"data",
          {{{"x", 1.0}, {"values", {a(0), a(1)}}},
           {{"x", 2.0}, {"values", {b(0), b(1)}}},
           {{"x", 3.0}, {"values", {c(0), c(1)}}}}}};

  Model::Networkproblem::Valuemap<2> valuemap_obj(values_json,"x");

  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = valuemap_obj(1.0);
    v2 = a;
    EXPECT_EQ(v, v2);
  }

  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = valuemap_obj(3.0);
    v2 = c;
    EXPECT_EQ(v, v2);
  }

  {
    Eigen::Vector2d v2;
    Eigen::Vector2d v;
    v = valuemap_obj(2.5);
    v2 = 0.5 * (b + c);
    // ACT
    EXPECT_EQ(v, v2);
  }

}

TEST(set_conditionTEST, check_order) {

  nlohmann::json values_json;

  Eigen::Matrix<double, 2, 1> M1;
  Eigen::Matrix<double, 2, 1> M2;
  Eigen::Matrix<double, 2, 1> M3;
  M1 << 0.0, 1.0;
  M2 << 1.0, 2.0;
  M3 << 2.0, 3.0;

  values_json
      = {{"id", "N213"},
         {"data",
          {{{"x", 1.0}, {"values", {0.0, 1.0}}},
           {{"x", 3.0}, {"values", {2.0, 3.0}}},
           {{"x", 2.0}, {"values", {1.0, 2.0}}}}}};

  std::vector<std::pair<double, Eigen::Matrix<double, 2, 1>>> control;

  control.push_back(std::make_pair(1.0, M1));
  control.push_back(std::make_pair(2.0, M2));
  control.push_back(std::make_pair(3.0, M3));

  EXPECT_EQ(control, Model::Networkproblem::Valuemap<2>::set_condition(values_json, "x"));

}

  TEST(set_conditionTEST, duplicate_x_value) {

    nlohmann::json values_json;

    values_json
        = {{"id", "N213"},
           {"data",
            {{{"x", 1.0}, {"values", {0.0, 1.0}}},
             {{"x", 1.0}, {"values", {1.0, 2.0}}},
             {{"x", 3.0}, {"values", {2.0, 3.0}}}}}};

    try {
      Model::Networkproblem::Valuemap<2>::set_condition(values_json, "x");
    } catch (std::exception &e) {
      EXPECT_THAT(
          e.what(),
          testing::HasSubstr(
              "The value 1.000000 appears twice in node with id N213 ."));
    }
}
