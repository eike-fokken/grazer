#include "Exception.hpp"
#include "Valuemap.hpp"
#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(set_conditionTEST, nothin_wrong) {

  nlohmann::json values_json;

  values_json
      = {{"id", "N213"},
         {"data",
          {{{"x", 1.0}, {"values", {1, 2}}},
           {{"x", 2.0}, {"values", {3, 4}}},
           {{"x", 3.0}, {"values", {5, 6}}}}}};

  EXPECT_NO_THROW(
      Model::Networkproblem::Valuemap<2>::set_condition(values_json, "x"));
}

TEST(set_conditionTEST, duplicate_x_value) {

  nlohmann::json values_json;

  values_json
      = {{"id", "N213"},
         {"data",
          {{{"x", 1.0}, {"values", {1,2}}},
           {{"x", 1.0}, {"values", {3,4}}},
           {{"x", 3.0}, {"values", {5,6}}}}}};

  EXPECT_THROW(Model::Networkproblem::Valuemap<2>::set_condition(
                               values_json, "x"), std::runtime_error);
}
