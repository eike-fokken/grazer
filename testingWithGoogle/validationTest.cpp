#include <gtest/gtest.h>

#include <json_validation.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

json TEST_SCHEMA = R"(
{
  "$schema": "http://json-schema.org/draft-07/schema",
  "title": "Test Schema",
  "type": "object",
  "required": ["my_number"],
  "properties": {
    "my_number": {"type": "number"},
    "my_string": {"type": "string"}
  }
}
)"_json;

TEST(testJsonValidation, happyPath) {
  json valid_data = R"(
    {
      "$schema":  "my_test_schema",
      "title": "Valid Test Data",
      "my_number": 6.2
    }
  )"
  
  validation::validate_json(valid_data, TEST_SCHEMA)  
}

