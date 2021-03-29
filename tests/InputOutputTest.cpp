#include "Input_output.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>

TEST(Input_output, absolute_file_path_in_root) {

  auto problem_root_path = std::filesystem::path("grazer/src/directory");
  auto filepath_true = std::filesystem::path("subdirectory/file");
  auto filepath_false = std::filesystem::path("/../subdirectory/file");

  EXPECT_TRUE(Aux_executable::absolute_file_path_in_root(
      problem_root_path, filepath_true));
  EXPECT_FALSE(Aux_executable::absolute_file_path_in_root(
      problem_root_path, filepath_false));

}

TEST(Input_output, prepare_output_dir) {


  // Creating a temporary directory
  std::string temp_dirname("temporary_dir");
  std::filesystem::path temp_dirpath(temp_dirname);
  std::filesystem::path absolute_dirpath(std::filesystem::absolute(temp_dirpath));
  if (not std::filesystem::exists(temp_dirpath)) {
    std::filesystem::create_directory(temp_dirname);
  } else {
    std::cout << "There is already a directory named " << temp_dirpath.string()
              << ", cannot execute the test. Please remove the directory at\n"
              << absolute_dirpath.string() << std::endl;
    FAIL();
  }

  // Testing a path that is not below the current working directory
  std::filesystem::path dirpath_false;
  dirpath_false = std::filesystem::path("..") / temp_dirpath;

  EXPECT_THROW(Aux_executable::prepare_output_dir(dirpath_false.string()).string(), std::runtime_error);

  // Testing if the output directory has a new, unique name
  // auto general_temp_dirname = std::filesystem::path(temp_dirname  + "_*");

  EXPECT_EQ(Aux_executable::prepare_output_dir(temp_dirname), temp_dirname);
  EXPECT_TRUE(std::filesystem::exists(temp_dirpath));
  // EXPECT_TRUE(std::filesystem::exists(general_temp_dirname));

  // Removing temporary directory after rename
  std::filesystem::remove(temp_dirpath);
  // std::filesystem::remove(general_temp_dirname);

  // Testing a path that does not point to a directory but to a file
  std::string temp_filename("temporary_file");
  std::filesystem::path temp_filepath(temp_filename);
  std::filesystem::path absolute_filepath(
      std::filesystem::absolute(temp_filepath));
  if (not std::filesystem::exists(temp_filepath)) {
    std::ofstream file(temp_filename);
  } else {
    std::cout << "There is already a file named " << temp_filepath.string()
              << ", cannot execute the test. Please remove the file at\n"
              << absolute_filepath.string() << std::endl;
    FAIL();
  }

  EXPECT_THROW(Aux_executable::prepare_output_dir(temp_filename), std::runtime_error);

  // Removing temporary file
  std::filesystem::remove(temp_filepath);
}

TEST(Input_output, extract_input_data){

  std::vector<std::string> vector_too_large;
  std::vector<std::string> vector_empty;
  std::vector<std::string> vector_size_one;
  nlohmann::json regular_json;
  std::string regular_json_name("problem_data.json");
  std::filesystem::path regular_json_path(regular_json_name);
  bool created_file_for_the_test(false);

  // Testing vector containing too many (>1) command arguments
  vector_too_large = {"str1", "str2"};
  EXPECT_THROW(Aux_executable::extract_input_data(vector_too_large), std::runtime_error);

  // Testing a vector containing a string that points to a regular file
  regular_json = {{"id", "M000"}, {"data", {1.0, 2.0}}};

  // Creating a temporary json file
  // absolute_path = std::filesystem::absolute(regular_json_path);
  if (not std::filesystem::exists(regular_json_path)) {
    std::ofstream file(regular_json_path);
    file << regular_json;
    created_file_for_the_test = true;

  } else if (!std::filesystem::is_regular_file(regular_json_path)) {
    std::cout << "There is already a file named " << regular_json_name
              << ", but it is not a regular file. Cannot execute the test." << std::endl;
    FAIL();
  }

  vector_size_one = {regular_json_name};
  EXPECT_EQ(std::filesystem::path("problem_data.json"), Aux_executable::extract_input_data(vector_size_one));

  // Testing empty vector
  vector_empty = {};
  EXPECT_EQ(std::filesystem::path("problem_data.json"),
            Aux_executable::extract_input_data(vector_empty));

  // Removing the regular file if it did not exist before the test
  // if (created_file_for_the_test){
  // std::filesystem::remove(regular_json_path);
  // }

}
