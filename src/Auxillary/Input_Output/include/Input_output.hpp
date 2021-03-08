#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

namespace Aux_executable {

  /// Makes a C++ vector of strings out of the C-style commandline arguments.
  std::vector<std::string> make_cmd_argument_vector(int argc, char **argv);

  /// Checks whether the given path filepath is inside of problem_root_path.
  ///
  /// If yes, returns a normalized path.
  /// If no, throws.
  bool
  absolute_file_path_in_root(const std::filesystem::path &problem_root_path,
                             const std::filesystem::path &filepath);


  /// Checks, whether the given output directory path is present.
  ///
  /// If not,
  /// returns the path. If yes, checks whether it is a directory. If yes, moves
  /// this directory and appends a unique string to it. If it is present, but
  /// not a directory, it throws an exception.
  std::filesystem::path prepare_output_dir(std::string output_dir);

  std::filesystem::path
  extract_input_data(std::vector<std::string> const &cmd_arguments);

} // namespace Aux_executable