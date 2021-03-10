#include <gtest/gtest.h>

#include <json_validation.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

class JsonContainer : public ::testing::Test {

public:
  const static json valid_schema = R"(
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

  const static json valid_data = R"(
    {
      "$schema":  "my_test_schema",
      "title": "Valid Test Data",
      "my_number": 6.2
    }
  )"_json;

  const static json missing_data = R"(
    {
      "$schema":  "my_test_schema",
      "title": "Valid Test Data",
      "my_string": "test"
    }
  )"_json;

  const static json invalid_data = R"(
    {
      "$schema":  "my_test_schema",
      "title": "Valid Test Data",
      "my_number": "test"
    }
  )"_json;

  const static json invalid_schema = R"( {"type": "invalid Schema"} )"_json;
};

TEST_F(JsonContainer, happyPath) {
  EXPECT_NO_THROW(validation::validate_json(valid_data, valid_schema));
}

TEST_F(JsonContainer, wrongData) {
  EXPECT_THROW(validation::validate_json(missing_data, valid_schema), std::runtime_error);
  EXPECT_THROW(validation::validate_json(invalid_data, valid_schema), std::runtime_error);
}

TEST_F(JsonContainer, invalidSchema) {
  EXPECT_THROW(validation::validate_json(valid_data, invalid_schema), std::runtime_error);
}