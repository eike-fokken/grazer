#include "Aux_json.hpp"
#indlude <Exception.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(Aux_json, replace_entry_with_json_from_file) {
  
  nlohmann::json aux_sub_json_test;
  nlohmann::json absolute_path_json;
  nlohmann::json relative_path_json;
  nlohmann::json wrong_path_json;

  std::string temp_json_name;
  std::string absolute_path;

  // Exemplary sub json file
  aux_sub_json_test = {{"id", "M000"},
                       {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};

  // Creating a temporary json file
  temp_json_name = "temporary_json.json";
  if (not std::filesystem::exists(temp_json_name)) {
    std::ofstream file(temp_json_name);
    file << aux_sub_json_test;
    absolute_path = std::filesystem::absolute(temp_json_name);
  } else {
    gthrow({"File Name already taken"});
  }


  // Testing jsons containing the absolute path
  absolute_path_json = {{"key", absolute_path}};
  aux_json::replace_entry_with_json_from_file(absolute_path_json, "key");
  EXPECT_EQ(aux_sub_json_test, absolute_path_json);


  // Testing jsons containing a relative path
  relative_path_json = {{"key", temp_json_name},
                        {"GRAZER_file_directory",
                         absolute_path + "/.."}};
  aux_json::replace_entry_with_json_from_file(relative_path_json, "key");
  EXPECT_EQ(aux_sub_json_test, relative_path_json["key"]);


  // Testing jsons containing a wrong path (e.g. non-existing path)
  wrong_path_json = {{"key", "/wrong/path.json"}};
  EXPECT_THROW(aux_json::replace_entry_with_json_from_file(wrong_path_json,"key"),
               std::runtime_error);

  // Removing temporary file
  // std::filesystem::remove(temp_json_name);

};

TEST(Aux_json, get_json_from_file_path) {

  std::string wrong_path = "/wrong/path.json";
  std::string right_path = "/Users/Tibor_Cornelli1/Desktop/nos.nosync/grazer/data/aux_sub_json_test.json";
  nlohmann::json aux_sub_json_test;

  EXPECT_ANY_THROW(aux_json::get_json_from_file_path(wrong_path));

  aux_sub_json_test = {{"id", "M000"},
                       {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};

  EXPECT_EQ(aux_sub_json_test,
            aux_json::get_json_from_file_path(right_path));

};
