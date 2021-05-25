#include <Exception.hpp>
#include <Input_output.hpp>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace io {
  using std::string;

  bool absolute_file_path_in_root(
      const std::filesystem::path &problem_root_path,
      const std::filesystem::path &filepath) {
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


  std::deque<std::string> args_as_deque(int argc, char **argv) {
    return std::deque<std::string>(argv + 1, argv + argc);
  }

  std::filesystem::path prepare_output_dir(std::filesystem::path output_dir) {

    if (!absolute_file_path_in_root(
            std::filesystem::current_path(), output_dir)) {
      gthrow(
          {"The output directory must be below the current working ",
           "directory, but it is not.\n", "Current working directory: ",
           std::filesystem::current_path().string(), "\n",
           "Chosen output directory: ", output_dir.string()});
    }

    if (std::filesystem::exists(output_dir)) {
      if (!std::filesystem::is_directory(output_dir)) {
        gthrow(
            {"The output directory, \"", output_dir.string(),
             "\" is present, but not a directory, I will abort now."});
      }
      auto ms_since_epoch
          = std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch());
      std::string milli = std::to_string(ms_since_epoch.count());
      std::string moved_output_dir;
      moved_output_dir = output_dir.string();
      moved_output_dir.append("_");
      moved_output_dir.append(milli);
      std::filesystem::rename(output_dir.string(), moved_output_dir);
      std::cout << "moved old directory " << output_dir << " to "
                << moved_output_dir << std::endl;
      std::cout << "result files will be saved to " << output_dir << std::endl;
    }
    std::filesystem::create_directory(output_dir);
    return output_dir;
  }

  std::filesystem::path
  extract_input_data(std::deque<std::string> const &cmd_arguments) {

    if (cmd_arguments.size() != 1) {
      gthrow(
          {"Grazer needs 1 argument: The problem directory.\n"
           "Aborting now."})
    }
    std::filesystem::path problem_directory = cmd_arguments[0];
    if (!std::filesystem::is_directory(problem_directory)) {
      gthrow(
          {"The given path\n\n",
           std::filesystem::absolute(problem_directory).string(),
           "\n\ndoes not point to a valid directory.  Maybe the name is "
           "misspelled."});
    }
    return problem_directory;
  }

} // namespace io
