#include <Exception.hpp>
#include <Input_output.hpp>
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
    if (counter >= 10) {
      gthrow(
          {"Couldn't aquire a unique filename in ",
           std::to_string(number_of_tries), " tries.\n",
           "Last tried filename was\n", unique_name.string(), "\nAborting..."});
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
             "\" is present, but not a directory, I will abort now."});
      }
    }

    fs::create_directory(output_dir);
    fs::path rel_filename("output.json");
    fs::path filename = output_dir / rel_filename;

    auto outputpath
        = fs::absolute(create_new_output_file(filename, attach_epoch_count));
    return outputpath;
  }

} // namespace io
