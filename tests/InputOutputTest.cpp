#include "Input_output.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(Input_output, absolute_file_path_in_root) {

  auto problem_root_path_true = std::filesystem::path("grazer/src/directory");
  auto problem_root_path_false
      = std::filesystem::path("grazer/src/../directory");
  auto filepath = std::filesystem::path("subdirectory/file");

  EXPECT_TRUE(Aux_executable::absolute_file_path_in_root(
      problem_root_path_true, filepath));
  EXPECT_FALSE(Aux_executable::absolute_file_path_in_root(
      problem_root_path_false, filepath));
}

TEST(Input_output, make_argument_vector) {}
