#include "Input_output.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

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

  EXPECT_THROW(Aux_executable::prepare_output_dir(dirpath_false.string()), std::runtime_error);

  // Removing temporary directory
  std::filesystem::remove(temp_dirpath);

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

