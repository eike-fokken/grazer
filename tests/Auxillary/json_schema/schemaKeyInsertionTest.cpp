#include "schema_key_insertion.hpp"
#include "test_io_helper.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

// Inheritance just for the name change (test fixture inherits name)
class schema_key_Insertion_Test : public CWD_To_Rand_Test_Dir {};

using nlohmann::json;
using std::filesystem::path;

TEST_F(schema_key_Insertion_Test, no_mod_if_no_schemas) {
  ASSERT_NO_THROW( // should not fail when there is no schema directory
      { Aux::schema::insert_schema_key_into_data_jsons(path(".")); });
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
