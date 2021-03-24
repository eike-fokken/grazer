#include <Exception.hpp>
#include <Input_output.hpp>
#include <filesystem>
#include <iostream>

namespace Aux_executable {

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

  std::vector<std::string> make_cmd_argument_vector(int argc, char **argv) {
    char **cmd_argument_pointer = argv;
    std::vector<std::string> cmd_arguments(
        cmd_argument_pointer + 1, cmd_argument_pointer + argc);
    return cmd_arguments;
  }

  std::filesystem::path prepare_output_dir(std::string output_dir_string) {

    std::filesystem::path output_dir(output_dir_string);

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
            {"The output directory, \"", output_dir_string,
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
  extract_input_data(std::vector<std::string> const &cmd_arguments) {

    std::string default_problem_data_filename = "problem_data.json";
    std::filesystem::path problem_data_file;
    if (cmd_arguments.size() > 1) {
      gthrow(
          {"Grazer needs 0 or 1 argument: The problem data file.\n"
           " If you provide no argument, the filename ",
           default_problem_data_filename,
           " in the directory, where Grazer was started, is assumed.\n"
           "Aborting now."})
    } else if (cmd_arguments.size() == 0) {
      problem_data_file = default_problem_data_filename;
    } else {
      problem_data_file = cmd_arguments[0];
    }
    if (!std::filesystem::is_regular_file(problem_data_file)) {
      gthrow(
          {"The given path ", problem_data_file.string(),
           " does not point to a regular file.  Maybe the name is "
           "misspelled."});
    }
    return problem_data_file;
  }

} // namespace Aux_executable
