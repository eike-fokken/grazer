#include "Input_output.hpp"
#include "test_io_helper.hpp"
#include <array>
#include <exception>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

class prepare_output_dirTEST : public ::testing::Test {
public:
  prepare_output_dirTEST() : path_changer(directory_creator.get_path()) {}

private:
  Directory_creator const directory_creator;
  Path_changer const path_changer;
};

class extract_input_dataTEST : public ::testing::Test {
public:
  extract_input_dataTEST() : path_changer(directory_creator.get_path()) {}

private:
  Directory_creator const directory_creator;
  Path_changer const path_changer;
};

TEST(absolute_file_path_in_rootTEST, wrong_path) {

  auto problem_root_path = std::filesystem::path("grazer/src/directory");
  auto filepath_false = std::filesystem::path("/../subdirectory/file");

  EXPECT_FALSE(
      io::absolute_file_path_in_root(problem_root_path, filepath_false));
}

TEST(absolute_file_path_in_rootTEST, right_path) {

  auto problem_root_path = std::filesystem::path("grazer/src/directory");
  auto filepath_true = std::filesystem::path("subdirectory/file");

  EXPECT_TRUE(io::absolute_file_path_in_root(problem_root_path, filepath_true));
}

TEST(make_cmd_argument_vectorTEST, two_arguments) {

  // The c++ standard requires that argv[argc]==nullptr.
  const char *const_argv[] = {"grazer", "arg1", "arg2", nullptr};
  int argc = 3;
  char **argv = const_cast<char **>(const_argv);

  std::vector<std::string> vec;
  vec = {"arg1", "arg2"};

  EXPECT_EQ(io::args_as_vector(argc, argv), vec);
}

TEST_F(prepare_output_dirTEST, path_not_below_current_dir) {

  std::filesystem::path temp_dirpath("temporary_dir");
  std::filesystem::path dirpath_false;

  std::filesystem::create_directory(temp_dirpath);

  // Testing a path that is not below the current working directory
  dirpath_false = std::filesystem::path("..") / temp_dirpath;

  EXPECT_THROW(
      [[maybe_unused]] auto result
      = io::prepare_output_dir(dirpath_false.string()),
      std::runtime_error);
}

TEST_F(prepare_output_dirTEST, directory_has_new_unique_name) {

  std::filesystem::path temp_dirpath("temporary_dir");

  // Creating a temporary directory within the new, empty directory
  std::filesystem::create_directory(temp_dirpath);

  // Testing if the output directory has a new, unique name
  //
  // If all goes well we catch the outputs to std::cout.
  std::string output;
  {
    std::stringstream buffer;
    Catch_cout catcher(buffer.rdbuf());
    EXPECT_EQ(
        io::prepare_output_dir(temp_dirpath.string()), temp_dirpath.string());
    EXPECT_TRUE(std::filesystem::exists(temp_dirpath));
    output = buffer.str();
  }
  // If the test failed, we present the output of the test.
  if (HasFailure()) {
    std::cout << "The test failed. It wrote the following to standard out:\n\n"
              << std::flush;
    std::cout << output << std::endl;
  }
}

TEST_F(prepare_output_dirTEST, path_points_to_file) {

  std::filesystem::path temp_filepath("temporary_file");

  // Testing a path that does not point to a directory but to a file
  std::ofstream file(temp_filepath.string());
  EXPECT_THROW(
      [[maybe_unused]] auto result
      = io::prepare_output_dir(temp_filepath.string()),
      std::runtime_error);
}

TEST_F(extract_input_dataTEST, input_vector_too_large) {

  std::vector<std::string> vector_too_large;

  // Testing vector containing too many (>1) command arguments
  vector_too_large = {"str1", "str2"};
  EXPECT_THROW(
      [[maybe_unused]] auto result = io::extract_input_data(vector_too_large),
      std::runtime_error);
}

TEST_F(extract_input_dataTEST, input_vector_works) {

  nlohmann::json regular_json;
  std::filesystem::path regular_json_path("test_pathname.json");
  std::vector<std::string> vector_size_one;

  // Testing a vector containing a string that points to a regular file
  regular_json = {{"id", "M000"}, {"data", {1.0, 2.0}}};
  std::ofstream file(regular_json_path);
  file << regular_json;
  vector_size_one = {regular_json_path.string()};
  EXPECT_EQ(
      std::filesystem::path("test_pathname.json"),
      io::extract_input_data(vector_size_one));
}

TEST_F(extract_input_dataTEST, input_vector_empty) {

  nlohmann::json regular_json;
  std::filesystem::path regular_json_path("problem_data.json");
  std::vector<std::string> vector_empty;

  // Creating json file
  regular_json = {{"id", "M000"}, {"data", {1.0, 2.0}}};
  std::ofstream file(regular_json_path);
  file << regular_json;

  // Testing empty vector
  vector_empty = {};
  EXPECT_EQ(regular_json_path, io::extract_input_data(vector_empty));
}
