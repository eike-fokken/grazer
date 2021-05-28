#include "schema_key_insertion.hpp"
#include "test_io_helper.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <iostream>

// Inheritance just for the name change (test fixture inherits name)
class schema_key_Insertion_Test : public CWD_To_Rand_Test_Dir {};

using nlohmann::json;
using std::filesystem::path;

TEST_F(schema_key_Insertion_Test, no_mod_if_no_schemas) {
  path problem_dir("problem");
  std::filesystem::create_directory(problem_dir);

  // create topology file which is not supposed to be modified
  path top_path = problem_dir / "topology.json";
  std::ofstream top_file(top_path);
  json top_json = R"({"key": "data"})"_json;
  top_file << top_json;
  top_file.close();

  ASSERT_NO_THROW( // should not fail when there is no schema directory
      { Aux::schema::insert_schema_key_into_data_jsons(path(".")); });

  // check that the file was not modified (since there was no schema for it)
  ASSERT_TRUE(std::filesystem::is_regular_file(top_path));
  std::ifstream ifs(top_path);
  json new_top_json = json::parse(ifs);
  ASSERT_TRUE(new_top_json.contains("key"));
  EXPECT_EQ(new_top_json["key"], "data");
  EXPECT_FALSE(new_top_json.contains("$schema"));
}

TEST_F(schema_key_Insertion_Test, correct_key_inserted) {
  path schemas("schemas");
  std::filesystem::create_directory(schemas);
  std::vector<std::string> types(
      {"topology", "boundary", "control", "initial"});
  for (std::string &type : types) {
    std::ofstream file(schemas / (type + "_schema.json"));
    file.close();
  }

  ASSERT_NO_THROW( // should not throw when data files are missing
      { Aux::schema::insert_schema_key_into_data_jsons(path(".")); });

  path problem_dir("problem");

  ASSERT_TRUE(std::filesystem::is_directory(problem_dir));
  for (std::string &type : types) {
    path file = problem_dir / (type + ".json");
    ASSERT_TRUE(std::filesystem::is_regular_file(file));

    json data_json;
    ASSERT_NO_THROW({
      std::ifstream infile(file);
      data_json = json::parse(infile);
    });
    ASSERT_TRUE(data_json.contains("$schema"));
    EXPECT_EQ(data_json["$schema"], "../schemas/" + type + "_schema.json");
  }
}

TEST_F(schema_key_Insertion_Test, no_modification_of_existing_data) {
  path problem_dir("problem");
  std::filesystem::create_directory(problem_dir);

  // create topology file which should retain all data
  path top_path = problem_dir / "topology.json";
  std::ofstream top_file(top_path);
  nlohmann::ordered_json top_json = R"({"key": "data", "key2": "data2"})"_json;
  top_file << top_json;
  top_file.close();

  // create initial file which is not supposed to be modified
  path init_path = problem_dir / "initial.json";
  std::ofstream init_file(init_path);
  nlohmann::ordered_json init_json = R"({"key": "data", "key2": "data2"})"_json;
  init_file << init_json;
  init_file.close();

  // create some schemas
  path schemas("schemas");
  std::filesystem::create_directory(schemas);
  std::vector<std::string> types({"topology", "boundary", "control"});
  for (std::string &type : types) {
    std::ofstream file(schemas / (type + "_schema.json"));
    file.close();
  }

  ASSERT_NO_THROW( // should not throw when data files are missing
      { Aux::schema::insert_schema_key_into_data_jsons(path(".")); });

  ASSERT_TRUE(std::filesystem::is_directory(problem_dir));

  ASSERT_TRUE(std::filesystem::is_regular_file(top_path));
  ASSERT_TRUE(std::filesystem::is_regular_file(init_path));

  std::ifstream ifs(top_path);
  nlohmann::ordered_json new_top_json = nlohmann::ordered_json::parse(ifs);

  // test that $schema key is inserted at the top and that order is retained
  auto it = new_top_json.cbegin();
  EXPECT_EQ(it.key(), "$schema");
  it++;
  EXPECT_EQ(it.key(), "key");
  it++;
  EXPECT_EQ(it.key(), "key2");

  std::ifstream init_ifs(init_path);
  nlohmann::ordered_json new_init_json
      = nlohmann::ordered_json::parse(init_ifs);
  EXPECT_EQ(new_init_json, init_json);
}
