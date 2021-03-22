#include "Aux_json.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(Aux_json, replace_entry_with_json_from_file) {


  nlohmann::json relative_path_json;
  nlohmann::json aux_sub_json_test;
  nlohmann::json wrong_path_json;


  // Exemplary sub json file
  aux_sub_json_test = {{"id", "M000"},
                       {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};

  // string pointing to the json file is relative
  // Not solved yet: how to set an "absolute" path within the grazer directory such that
  // it is absolute for everyone? The path below ony works for my (Tibors) file system
  relative_path_json = {{"key", "data/aux_sub_json_test.json"},
                        {"GRAZER_file_directory","/Users/Tibor_Cornelli1/Desktop/nos.nosync/grazer"}};

  aux_json::replace_entry_with_json_from_file(relative_path_json, "key");

  EXPECT_EQ(aux_sub_json_test, relative_path_json["key"]);


  // Invalid string (e.g. pointing to a non-existing file)
  wrong_path_json = {{"key", "/wrong/path.json"}};

  EXPECT_THROW(aux_json::replace_entry_with_json_from_file(wrong_path_json,"key"), std::runtime_error);


};

TEST(Aux_json, get_json_from_file_path) {


  std::string wrong_path = "/wrong/path.json";
  std::string right_path = "/Users/Tibor_Cornelli1/Desktop/nos.nosync/grazer/data/aux_sub_json_test.json";
  nlohmann::json aux_sub_json_test;

  EXPECT_ANY_THROW(aux_json::get_json_from_file_path(wrong_path));

  aux_sub_json_test = {{"id", "M000"},
                       {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};

  EXPECT_EQ(aux_sub_json_test,aux_json::get_json_from_file_path(right_path));

};
