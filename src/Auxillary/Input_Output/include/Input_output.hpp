#pragma once
#include <deque>
#include <filesystem>
#include <functional>
#include <variant>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace io {
  using std::string;

  class Option {
    // no default means required
    std::optional<string> default_value;
  public:
    string name;
    string description; // help text
    Option(
        string name, std::optional<string> default_value,
        string description = "");
  };

  using program
      = std::function<int(std::deque<string>, std::map<string, string>)>;

  class Command {
    program const program;
    std::vector<Option> const options;
  public:
    string const name;
    string const description; // help text
    int execute(std::deque<string> arguments, string group_name="") const;

    /**
     * @brief Create a Command Group
     *
     * @param subcommands
     * @param name
     * @param description
     */
    Command(
        std::vector<Command> const subcommands, string const name,
        string const description = "");

    /**
     *@brief Create a normal Command
     *
     * @param prgrm
     * @param options
     * @param name
     * @param description
     */
    Command(
        io::program const program, std::vector<Option> const options,
        string const name, string const description);
  };

  void print_help(
      string program_name, string program_description,
      std::vector<Option> options, std::vector<Command> commands);

  std::deque<std::string> args_as_deque(int argc, char **argv);

  /// Checks whether the given path filepath is inside of problem_root_path.
  ///
  /// If yes, returns a normalized path.
  /// If no, throws.
  bool absolute_file_path_in_root(
      const std::filesystem::path &problem_root_path,
      const std::filesystem::path &filepath);

  /// Checks, whether the given output directory path is present.
  ///
  /// If not,
  /// returns the path. If yes, checks whether it is a directory. If yes, moves
  /// this directory and appends a unique string to it. If it is present, but
  /// not a directory, it throws an exception.
  std::filesystem::path prepare_output_dir(std::string output_dir);

  std::filesystem::path
  extract_input_data(std::deque<std::string> const &cmd_arguments);

} // namespace io
