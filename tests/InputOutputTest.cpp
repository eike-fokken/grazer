#include "Input_output.hpp"
#include <array>
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

TEST(Input_output, make_cmd_argument_vector) {

  const char *const_argv[] = {"grazer","arg1","arg2", nullptr};
  int argc = 3;
  char ** argv = const_cast<char **>(const_argv);

  std::vector<std::string> vec;
  vec = {"arg1", "arg2"};

  EXPECT_EQ(Aux_executable::make_cmd_argument_vector(argc,argv), vec);

}

TEST(Input_output, prepare_output_dir) {

  std::filesystem::path main_test_dir_path("main_test_dir");
  std::filesystem::path temp_dirpath("temporary_dir");
  std::filesystem::path temp_filepath("temporary_file");
  std::filesystem::path dirpath_false;

  // Creating a main test directory and changing into it automatically
  if (not std::filesystem::exists(main_test_dir_path)) {
    std::filesystem::create_directory(main_test_dir_path.string());
    std::filesystem::current_path(
        std::filesystem::absolute(main_test_dir_path));
  } else {
    std::cout << "There is already a directory named "
              << main_test_dir_path.string()
              << ", cannot execute the test. Please remove the directory at\n"
              << std::filesystem::absolute(main_test_dir_path).string()
              << std::endl;
    FAIL();
  }

  // Creating a temporary directory within the new, empty directory
  std::filesystem::create_directory(temp_dirpath.string());

  // Testing a path that is not below the current working directory
  dirpath_false = std::filesystem::path("..") / temp_dirpath;
  EXPECT_THROW(Aux_executable::prepare_output_dir(dirpath_false.string()).string(), std::runtime_error);

  // Testing if the output directory has a new, unique name
  EXPECT_EQ(Aux_executable::prepare_output_dir(temp_dirpath.string()), temp_dirpath.string());
  EXPECT_TRUE(std::filesystem::exists(temp_dirpath));

  // Removing temporary directory after rename
  std::filesystem::remove(temp_dirpath);

  // Testing a path that does not point to a directory but to a file
  std::ofstream file(temp_filepath.string());
  EXPECT_THROW(Aux_executable::prepare_output_dir(temp_filepath.string()), std::runtime_error);

  // Removing all files/folders in main test directory
  std::filesystem::current_path(std::filesystem::path(".."));
  std::filesystem::remove_all(main_test_dir_path);
  std::filesystem::remove(main_test_dir_path);
}

TEST(Input_output, extract_input_data){

  std::filesystem::path main_test_dir_path("main_test_dir");
  nlohmann::json regular_json;
  std::filesystem::path regular_json_path("problem_data.json");
  std::vector<std::string> vector_too_large;
  std::vector<std::string> vector_empty;
  std::vector<std::string> vector_size_one;

  // Creating a main test directory and changing into it automatically
  if (not std::filesystem::exists(main_test_dir_path)) {
    std::filesystem::create_directory(main_test_dir_path.string());
    std::filesystem::current_path(
        std::filesystem::absolute(main_test_dir_path));
  } else {
    std::cout << "There is already a directory named "
              << main_test_dir_path.string()
              << ", cannot execute the test. Please remove the directory at\n"
              << std::filesystem::absolute(main_test_dir_path).string()
              << std::endl;
    FAIL();
  }

  // Testing vector containing too many (>1) command arguments
  vector_too_large = {"str1", "str2"};
  EXPECT_THROW(Aux_executable::extract_input_data(vector_too_large), std::runtime_error);

  // Testing a vector containing a string that points to a regular file
  regular_json = {{"id", "M000"}, {"data", {1.0, 2.0}}};
  std::ofstream file(regular_json_path);
  file << regular_json;
  vector_size_one = {regular_json_path.string()};
  EXPECT_EQ(std::filesystem::path("problem_data.json"), Aux_executable::extract_input_data(vector_size_one));

  // Testing empty vector
  vector_empty = {};
  EXPECT_EQ(std::filesystem::path("problem_data.json"),
            Aux_executable::extract_input_data(vector_empty));

  // Removing 'main_test_dir'
  std::filesystem::current_path(std::filesystem::path(".."));
  std::filesystem::remove_all(main_test_dir_path);
  std::filesystem::remove(main_test_dir_path);
}
