/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#include "Input_output.hpp"
#include "test_io_helper.hpp"
#include <array>
#include <deque>
#include <exception>
#include <fstream>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// Inheritance just for the name change (test fixture inherits name)
class prepare_output_dirTEST : public CWD_To_Rand_Test_Dir {};

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

// TEST_F(prepare_output_dirTEST, path_not_below_current_dir) {

//   std::filesystem::path temp_dirpath("temporary_dir");
//   std::filesystem::path dirpath_false;

//   std::filesystem::create_directory(temp_dirpath);

//   // Testing a path that is not below the current working directory
//   dirpath_false = std::filesystem::path("..") / temp_dirpath;

//   EXPECT_THROW(
//       [[maybe_unused]] auto result
//       = io::prepare_output_dir(dirpath_false.string()),
//       std::runtime_error);
// }

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
  // EXPECT_THROW(
  //     [[maybe_unused]] auto result
  //     = io::prepare_output_dir(temp_filepath.string()),
  //     std::runtime_error);

  try {
    [[maybe_unused]] auto result
        = io::prepare_output_dir(temp_filepath.string());
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("The output directory, \"temporary_file\" "
                                     "is present, but not a directory"));
  }
}
