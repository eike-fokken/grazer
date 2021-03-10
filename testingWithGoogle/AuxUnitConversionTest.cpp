#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <unit_conversion.hpp>

namespace unit = Aux::unit;
using nlohmann::json;

TEST(unitParser, directCall) {
  json metres_10 = R"({
    {"value": 10},
    {"unit": "m"}
  })"_json; 
  json deca_metres_1 = R"({
    {"value": 1},
    {"unit": "dam"}
  })"_json;
  auto si_length = unit::parse_unit(metres_10, "m");

  EXPECT_EQ(si_length, 10.0);
  EXPECT_EQ(si_length, unit::parse_unit(deca_metres_1, "m"));
}