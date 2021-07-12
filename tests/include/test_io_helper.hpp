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

#pragma once
#include <filesystem>
#include <gtest/gtest.h>

struct Catch_cout {
  Catch_cout(std::streambuf *new_buffer);

  ~Catch_cout();

private:
  std::streambuf *old;
};

class Directory_creator {
public:
  Directory_creator(std::string testdirname);
  Directory_creator();

  ~Directory_creator();

  std::filesystem::path const &get_path() const;

private:
  std::filesystem::path const main_test_dir_path;
  static std::filesystem::path create_testdir(std::string directory_name);
};

class Path_changer {
public:
  Path_changer(std::filesystem::path testdir);
  ~Path_changer();

private:
  std::filesystem::path change_to_testdir(std::filesystem::path testdir);

  std::filesystem::path const old_current_directory;
};

class CWD_To_Rand_Test_Dir : public ::testing::Test {
public:
  CWD_To_Rand_Test_Dir() : path_changer(directory_creator.get_path()) {}

private:
  Directory_creator const directory_creator;
  Path_changer const path_changer;
};
