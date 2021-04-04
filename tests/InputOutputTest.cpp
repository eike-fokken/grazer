#include "Catch_cout.hpp"
#include "Input_output.hpp"
#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <ostream>
#include <sstream>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>

class Directory_creator {
public:
  Directory_creator(std::string testdirname) :
      main_test_dir_path(create_testdir(testdirname)) {}

  ~Directory_creator() {
    try {
      std::filesystem::remove_all(main_test_dir_path);
    } catch (...) {
      std::cout << "Couldn't remove " << main_test_dir_path.string() << ".\n\n"
                << "###############################################\n"
                << "Please remove it yourself!\n"
                << "###############################################\n\n"
                << std::flush;
    }
  }

  std::filesystem::path const &get_path() const { return main_test_dir_path; }

private:
  std::filesystem::path const main_test_dir_path;
  static std::filesystem::path create_testdir(std::string directory_name) {
    auto full_path = std::filesystem::temp_directory_path()
                     / std::filesystem::path(directory_name);
    auto new_dir = std::filesystem::create_directory(full_path);
    if (not new_dir) {
      std::ostringstream o;
      o << "There is already a directory named " << full_path.string()
        << ", cannot execute the test. Please remove the directory at\n"
        << full_path.string() << std::endl;
      throw std::runtime_error(o.str());
    }
    return full_path;
  }
};

class Path_changer {
public:
  Path_changer(std::filesystem::path testdir) :
      old_current_directory(change_to_testdir(testdir)) {}

  ~Path_changer() {
    try {
      std::filesystem::current_path(old_current_directory);
    } catch (...) {
      std::cout << "Couldn't change back to the old directory!\n"
                << "Swallowing the exception\n"
                << "to make sure the test directory is removed, if possible."
                << std::endl;
    }
  }

private:
  std::filesystem::path change_to_testdir(std::filesystem::path testdir) {
    auto curr_dir = std::filesystem::current_path();
    std::filesystem::current_path(testdir);
    return curr_dir;
  }

  std::filesystem::path const old_current_directory;
};

class prepare_output_dirTEST : public ::testing::Test {
public:
  prepare_output_dirTEST() :
      directory_creator("Grazer_testdir"),
      path_changer(directory_creator.get_path()) {}

private:
  Directory_creator const directory_creator;
  Path_changer const path_changer;
};

class extract_input_dataTEST : public ::testing::Test {
public:
  extract_input_dataTEST() :
      directory_creator("Grazer_testdir"),
      path_changer(directory_creator.get_path()) {}

private:
  Directory_creator const directory_creator;
  Path_changer const path_changer;
};

TEST(absolute_file_path_in_rootTEST, wrong_path) {

  auto problem_root_path = std::filesystem::path("grazer/src/directory");
  auto filepath_false = std::filesystem::path("/../subdirectory/file");

  EXPECT_FALSE(Aux_executable::absolute_file_path_in_root(
      problem_root_path, filepath_false));
}

TEST(absolute_file_path_in_rootTEST, right_path) {

  auto problem_root_path = std::filesystem::path("grazer/src/directory");
  auto filepath_true = std::filesystem::path("subdirectory/file");

  EXPECT_TRUE(Aux_executable::absolute_file_path_in_root(
      problem_root_path, filepath_true));
}

TEST(make_cmd_argument_vectorTEST, two_arguments) {

  // The c++ standard requires that argv[argc]==nullptr.
  const char *const_argv[] = {"grazer", "arg1", "arg2", nullptr};
  int argc = 3;
  char **argv = const_cast<char **>(const_argv);

  std::vector<std::string> vec;
  vec = {"arg1", "arg2"};

  EXPECT_EQ(Aux_executable::make_cmd_argument_vector(argc, argv), vec);
}

TEST_F(prepare_output_dirTEST, path_not_below_current_dir) {

  std::filesystem::path temp_dirpath("temporary_dir");
  std::filesystem::path dirpath_false;

  std::filesystem::create_directory(temp_dirpath);

  // Testing a path that is not below the current working directory
  dirpath_false = std::filesystem::path("..") / temp_dirpath;

  EXPECT_THROW(
      [[maybe_unused]] auto result
      = Aux_executable::prepare_output_dir(dirpath_false.string()),
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
        Aux_executable::prepare_output_dir(temp_dirpath.string()),
        temp_dirpath.string());
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
      = Aux_executable::prepare_output_dir(temp_filepath.string()),
      std::runtime_error);
}

TEST_F(extract_input_dataTEST, input_vector_too_large) {

  std::vector<std::string> vector_too_large;

  // Testing vector containing too many (>1) command arguments
  vector_too_large = {"str1", "str2"};
  EXPECT_THROW(
      [[maybe_unused]] auto result
      = Aux_executable::extract_input_data(vector_too_large),
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
      Aux_executable::extract_input_data(vector_size_one));
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
  EXPECT_EQ(
      regular_json_path, Aux_executable::extract_input_data(vector_empty));
}
