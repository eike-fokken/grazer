#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <unit_conversion.hpp>

namespace unit = Aux::unit;
using nlohmann::json;

TEST(unitParser, directCall) {
  json metres_10 = R"({
    "value": 10,
    "unit": "m"
  })"_json; 
  json deca_metres_1 = R"({
    "value": 1,
    "unit": "dam"
  })"_json;
  json ten_thousand_micro_metres_1000 = json::parse(
    "{\"value\": 1000,\"unit\": \"1e4 \u00B5m\"}"
  );

  auto si_length = unit::parse_unit(metres_10, "m");

  // expect_eq(si_length, 10.0);
  // expect_eq(si_length, unit::parse_unit(deca_metres_1, "m"));
  EXPECT_EQ(si_length, unit::parse_unit(ten_thousand_micro_metres_1000, "m"));
}