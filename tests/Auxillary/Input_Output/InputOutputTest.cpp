#include "Input_output.hpp"
#include "test_io_helper.hpp"
#include <array>
#include <deque>
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

  // Testing vector containing too many (>1) command arguments
  std::deque<std::string> too_many_args = {"str1", "str2"};

  EXPECT_THROW(
      [[maybe_unused]] auto result = io::extract_input_data(too_many_args),
      std::runtime_error);
}

TEST_F(extract_input_dataTEST, input_vector_works) {
  std::filesystem::path dir_path = "test_pathname";
  std::filesystem::create_directory(dir_path);
  EXPECT_EQ(dir_path, io::extract_input_data({dir_path.string()}));
}

TEST_F(extract_input_dataTEST, input_vector_empty) {
  // Testing empty vector
  std::deque<std::string> vector_empty = {};
  EXPECT_THROW(
      [[maybe_unused]] auto result = io::extract_input_data(vector_empty),
      std::runtime_error);
}
