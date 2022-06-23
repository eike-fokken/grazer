/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "Input_output.hpp"
#include "Exception.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

namespace io {
  namespace fs = std::filesystem;

  bool absolute_file_path_in_root(
      const fs::path &problem_root_path, const fs::path &filepath) {
    auto absolute_path = (problem_root_path / filepath).lexically_normal();

    auto [root_end_iterator, ignored_value] = std::mismatch(
        problem_root_path.begin(), problem_root_path.end(),
        absolute_path.begin());

    if (root_end_iterator != problem_root_path.end()) {
      return false;
    } else {
      return true;
    }
  }

  fs::path attach_epoch_count(fs::path const &filename) {
    auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    std::string milli = std::to_string(ms_since_epoch.count());

    auto directory = filename.parent_path();

    auto extension = filename.extension().string();
    auto new_name = filename.stem().string();
    new_name.append("_");
    new_name.append(milli);
    new_name.append(extension);
    fs::path full_filename(new_name);
    fs::path file_path_with_epoch = directory / full_filename;
    return file_path_with_epoch;
  }

  fs::path create_new_output_file(
      fs::path const &filename, filename_generator generator) {
    fs::path unique_name;
    FILE *fp = nullptr;
    int const number_of_tries = 10;
    int counter = 0;
    do {
      unique_name = generator(filename);
      fp = std::fopen(unique_name.string().c_str(), "wx");
      if (fp) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      ++counter;
    } while (counter < number_of_tries);
    if (counter >= number_of_tries) {
      gthrow(
          {"Couldn't aquire a unique filename in ",
           std::to_string(number_of_tries), " tries.\n",
           "Last tried filename was\n", unique_name.string(), "\n"});
    }
    std::fclose(fp);
    return unique_name;
  }

  fs::path prepare_output_file(fs::path directory_path) {
    fs::path rel_output_dir("output");
    fs::path output_dir = directory_path / rel_output_dir;
    if (fs::exists(output_dir)) {
      if (not fs::is_directory(output_dir)) {
        gthrow(
            {"The output directory, \"", output_dir.string(),
             "\" is present, but not a directory."});
      }
    }

    fs::create_directory(output_dir);
    fs::path rel_filename("output.json");
    fs::path filename = output_dir / rel_filename;

    auto outputpath
        = fs::absolute(create_new_output_file(filename, attach_epoch_count));
    return outputpath;
  }

  std::string millisecond_datetime_timestamp() {
    auto nowtime = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(nowtime);

    auto time_up_to_seconds = std::chrono::system_clock::from_time_t(t);

    auto ms_since_second
        = std::chrono::duration_cast<std::chrono::milliseconds>(
            nowtime - time_up_to_seconds);

    auto ms_string = std::to_string(ms_since_second.count());
    char mbstr[32];
    if (std::strftime(
            mbstr, sizeof(mbstr), "%Y.%m.%d_%H:%M:%S", std::localtime(&t))) {
      std::string timestring(mbstr);

      std::string full_timestring = timestring + "." + ms_string;
      return full_timestring;
    } else {
      gthrow(
          {"std::strftime somehow failed. This is a bug, please report it on "
           "github!"});
    }
  }

  std::filesystem::path unique_output_directory(
      std::filesystem::path const &outer_output_directory,
      dirname_generator dirname_generator_funtion) {
    int number_of_tries = 20;
    for (int counter = 0; counter != number_of_tries; ++counter) {
      std::filesystem::path absolute_unique_directory_path
          = outer_output_directory
            / std::filesystem::path(dirname_generator_funtion());
      if (std::filesystem::create_directory(absolute_unique_directory_path)) {
        return absolute_unique_directory_path;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    gthrow(
        {"Failed to create a unique output directory in ",
         std::to_string(number_of_tries), ".\n", "Are you running more than ",
         std::to_string(number_of_tries), " instances of grazer in parallel?"});
  }

  void prepare_output_directory(
      std::filesystem::path const &output_directory,
      std::filesystem::path const &problem_directory,
      std::vector<std::string> filenames_to_create) {
    namespace fs = std::filesystem;
    fs::path problem_files = output_directory / fs::path("problem");
    const auto copyOptions = fs::copy_options::recursive;
    fs::copy(problem_directory, problem_files, copyOptions);

    for (auto const &filename : filenames_to_create) {
      auto filepath = output_directory / fs::path(filename);
      auto fp = std::fopen(filepath.string().c_str(), "wx");
      if (not fp) {
        auto absolute_path = fs::absolute(filepath);
        gthrow({"Failed to create file: ", absolute_path.string()});
      }
      std::fclose(fp);
    }
  }

} // namespace io
