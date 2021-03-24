#include "Aux_json.hpp"
#include <Exception.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

// #include <iostream>

#define pln std::cout << __LINE__ << std::endl;

TEST(Aux_json, replace_entry_with_json_from_file) {
  
  nlohmann::json aux_sub_json_test;
  nlohmann::json object_json_test;
  nlohmann::json absolute_path_json;
  nlohmann::json relative_path_json;
  nlohmann::json wrong_path_json;

  std::string temp_json_name;
  std::filesystem::path absolute_path;

  // Exemplary sub json file
  aux_sub_json_test = {{"id", "M000"},
                       {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};
  pln
  // Creating a temporary json file
  temp_json_name = "temporary_json_test1.json";
  std::filesystem::path temp_json_path(temp_json_name);
  absolute_path = std::filesystem::absolute(temp_json_path);
  pln
  if (not std::filesystem::exists(temp_json_path)) {
    pln
    std::ofstream file(temp_json_path);
    pln
    file << aux_sub_json_test;
    pln
  } else {
    pln
    std::cout << "There is already a file named " << temp_json_path.string()
              << ", cannot execute the test. Please remove the file at\n"
              << absolute_path.string()
              << std::endl;
    pln
    FAIL();
  }
  pln

  // Testing jsons containing an Object
  object_json_test = {{"key", aux_sub_json_test}};
  aux_json::replace_entry_with_json_from_file(object_json_test, "key");
  EXPECT_EQ(aux_sub_json_test, object_json_test["key"]);


  // Testing jsons containing the absolute path
  absolute_path_json = {{"key", absolute_path.string()}};
  aux_json::replace_entry_with_json_from_file(absolute_path_json, "key");
  EXPECT_EQ(aux_sub_json_test, absolute_path_json["key"]);

  // Testing jsons containing a relative path
  relative_path_json = {{"key", temp_json_path.string()},
                         {"GRAZER_file_directory",
                          std::filesystem::current_path().string()}};
  aux_json::replace_entry_with_json_from_file(relative_path_json, "key");
  EXPECT_EQ(aux_sub_json_test, relative_path_json["key"]);


  // Testing jsons containing a wrong path (e.g. non-existing path)
  wrong_path_json = {{"key", "/wrong/path.json"}};
  EXPECT_THROW(aux_json::replace_entry_with_json_from_file(wrong_path_json,"key"),
               std::runtime_error);

  // Removing temporary file
  pln
  std::filesystem::remove(temp_json_path);
  pln
}

TEST(Aux_json, get_json_from_file_path) {

  std::string temp_json_name;
  std::string wrong_path = "/wrong/path.json";
  std::filesystem::path absolute_path;
  nlohmann::json aux_sub_json_test;


  // Testing wrong paths
  EXPECT_ANY_THROW(aux_json::get_json_from_file_path(wrong_path));


  // Testing right path
  aux_sub_json_test = {{"id", "M000"},
                       {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};

  // Creating a temporary json file
  temp_json_name = "temporary_json_test2.json";
  absolute_path = std::filesystem::absolute(temp_json_name);
  if (not std::filesystem::exists(temp_json_name)) {
    std::ofstream file(temp_json_name);
    file << aux_sub_json_test;
  } else {
    std::cout << "There is already a file named " << temp_json_name
              << ", cannot execute the test. Please remove the file at\n"
              << absolute_path.string()
              << std::endl;
    FAIL();
  }

  EXPECT_EQ(aux_sub_json_test,
            aux_json::get_json_from_file_path(absolute_path));


  // Removing temporary file
  std::filesystem::remove(temp_json_name);

}
