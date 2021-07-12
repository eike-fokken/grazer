/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#include "Aux_json.hpp"
#include "test_io_helper.hpp"
#include <exception>
#include <filesystem>
#include <fstream>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

class Aux_json_Test : public CWD_To_Rand_Test_Dir {};

TEST_F(Aux_json_Test, replace_entry_with_json_from_file) {

  nlohmann::json aux_sub_json_test;
  nlohmann::json object_json_test;
  nlohmann::json absolute_path_json;
  nlohmann::json relative_path_json;
  nlohmann::json wrong_path_json;

  std::string temp_json_name;
  std::filesystem::path absolute_path;

  // Exemplary sub json file
  aux_sub_json_test
      = {{"id", "M000"}, {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};
  // Creating a temporary json file
  temp_json_name = "temporary_json_test1.json";
  std::filesystem::path temp_json_path(temp_json_name);
  absolute_path = std::filesystem::absolute(temp_json_path);
  if (not std::filesystem::exists(temp_json_path)) {
    std::ofstream file(temp_json_path);
    file << aux_sub_json_test;
  } else {
    std::cout << "There is already a file named " << temp_json_path.string()
              << ", cannot execute the test. Please remove the file at\n"
              << absolute_path.string() << std::endl;
    FAIL();
  }

  // Testing jsons containing an Object
  object_json_test = {{"key", aux_sub_json_test}};
  aux_json::replace_entry_with_json_from_file(object_json_test, "key");
  EXPECT_EQ(aux_sub_json_test, object_json_test["key"]);

  // Testing jsons containing the absolute path
  absolute_path_json = {{"key", absolute_path.string()}};
  aux_json::replace_entry_with_json_from_file(absolute_path_json, "key");
  EXPECT_EQ(aux_sub_json_test, absolute_path_json["key"]);

  // Testing jsons containing a relative path
  relative_path_json
      = {{"key", temp_json_path.string()},
         {"GRAZER_file_directory", std::filesystem::current_path().string()}};
  aux_json::replace_entry_with_json_from_file(relative_path_json, "key");
  EXPECT_EQ(aux_sub_json_test, relative_path_json["key"]);

  // Testing jsons containing a wrong path (e.g. non-existing path)
  std::string wrong_path = "wrong/path.json";

  std::string json_path_string = std::filesystem::absolute(wrong_path).string();

  wrong_path_json = {{"key", json_path_string}};

  try {
    aux_json::replace_entry_with_json_from_file(wrong_path_json, "key");
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      "The file \n" + wrong_path_json["key"].get<std::string>()
                      + "\n does not exist!"));
  }
}

TEST_F(Aux_json_Test, get_json_from_file_path) {

  std::string temp_json_name;
  std::string wrong_path = "wrong/path.json";
  std::filesystem::path absolute_path;
  nlohmann::json aux_sub_json_test;

  auto json_pathstring = std::filesystem::path(wrong_path);
  auto json_absolute_path = std::filesystem::absolute(json_pathstring);

  // Testing wrong paths
  try {
    aux_json::get_json_from_file_path(wrong_path);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      "The file \n" + json_absolute_path.string()
                      + "\n does not exist!"));
  }

  // Testing right path
  aux_sub_json_test
      = {{"id", "M000"}, {"data", {{{"time", 0}, {"values", {1.0, 2.0}}}}}};

  // Creating a temporary json file
  temp_json_name = "temporary_json_test2.json";
  absolute_path = std::filesystem::absolute(temp_json_name);
  if (not std::filesystem::exists(temp_json_name)) {
    std::ofstream file(temp_json_name);
    file << aux_sub_json_test;
  } else {
    std::cout << "There is already a file named " << temp_json_name
              << ", cannot execute the test. Please remove the file at\n"
              << absolute_path.string() << std::endl;
    FAIL();
  }

  EXPECT_EQ(
      aux_sub_json_test, aux_json::get_json_from_file_path(absolute_path));
}
