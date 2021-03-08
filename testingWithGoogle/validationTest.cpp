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
  )"_json;
  
  ASSERT_NO_THROW(validation::validate_json(valid_data, TEST_SCHEMA));
}

TEST(testJsonValidation, wrongData) {
  json missing_data = R"(
    {
      "$schema":  "my_test_schema",
      "title": "Valid Test Data",
      "my_string": "test"
    }
  )"_json;
  
  EXPECT_THROW(validation::validate_json(missing_data, TEST_SCHEMA), std::runtime_error);

  json invalid_data = R"(
    {
      "$schema":  "my_test_schema",
      "title": "Valid Test Data",
      "my_number": "test"
    }
  )"_json;
  
  EXPECT_THROW(validation::validate_json(invalid_data, TEST_SCHEMA), std::runtime_error);
}

TEST(testJsonValidation, invalidSchema) {
  json invalid_schema = R"( {"type": "invalid Schema"} )"_json;
  json data = R"( {"$schema": "my_test_schema"})"_json;

  EXPECT_THROW(validation::validate_json(data, invalid_schema), std::runtime_error);
}