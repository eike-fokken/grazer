#include "Exception.hpp"
#include "Netfactory.hpp"
#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(Initialvalue, Operator) {

  nlohmann::json topology_json;

  topology_json
      = {{"id", "N213"},
         {"data",
          {{{"x", 1.0}, {"values", {0, 1}}},
           {{"x", 2.0}, {"values", {0, 1}}},
           {{"x", 3.0}, {"values", {0, 1}}}}}};

}
