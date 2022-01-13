#include "Input_output.hpp"
#include "test_io_helper.hpp"
#include <array>
#include <deque>
#include <exception>
#include <filesystem>
#include <fstream>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// Inheritance just for the name change (test fixture inherits name)
class create_new_output_fileTEST : public CWD_To_Rand_Test_Dir {};
class prepare_output_fileTEST : public CWD_To_Rand_Test_Dir {};
// Inheritance just for the name change (test fixture inherits name)
class extract_input_dataTEST : public CWD_To_Rand_Test_Dir {};

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

TEST_F(create_new_output_fileTEST, find_no_unique_name) {

  std::filesystem::path filename1("myfilename");
  auto filenamegetter = [](std::filesystem::path const &) {
    return std::filesystem::path("myfilename2");
  };

  std::ofstream output(filenamegetter(""));
  try {
    [[maybe_unused]] auto result
        = io::create_new_output_file(filename1, filenamegetter);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("Couldn't aquire a unique filename"));
  }
}

TEST_F(create_new_output_fileTEST, succeed) {

  std::filesystem::path filename1("myfilename");

  auto filenamegetter = [](std::filesystem::path const &) {
    return std::filesystem::path("myfilename2");
  };
  auto filename2(filenamegetter(filename1));
  std::ofstream output(filename1);

  auto path = io::create_new_output_file(filename1, filenamegetter);

  EXPECT_EQ(filename2.string(), path.string());
}

TEST_F(prepare_output_fileTEST, output_not_a_directory) {

  std::filesystem::path temp_dirpath("output");

  // Creating a temporary directory within the offending directory
  std::ofstream bad(temp_dirpath);
  try {
    [[maybe_unused]] auto result
        = io::prepare_output_file(std::filesystem::current_path());
    std::cout << result.string() << std::endl;
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("is present, but not a directory"));
  }
}

TEST(millisecond_datetime_timestamp, happy_path) {

  auto datetime_string = io::millisecond_datetime_timestamp();
  auto it = std::find(datetime_string.begin(), datetime_string.end(), ' ');
  std::cout << "datetime string with milliseconds: " << datetime_string
            << std::endl;
  EXPECT_EQ(it, datetime_string.end());
}
