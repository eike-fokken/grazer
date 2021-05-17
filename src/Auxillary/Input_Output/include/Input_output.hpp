#pragma once
#include <any>
#include <deque>
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace io {
  using std::string;

  class Option {
    // no default means required
    std::optional<std::any> const default_value;
    std::function<std::any(std::vector<string>)> const parser;

  public:
    string const name;
    string const description; // help text
    std::vector<string> const alias;
    uint16_t const nargs;
    std::any operator()(std::vector<string> const arguments) const;
    Option(
        std::optional<string> default_value, string name,
        string description = "",
        std::vector<string> alias = std::vector<string>());
  };

  using program
      = std::function<int(std::deque<string>, std::map<string, std::any>)>;

  class Command {
    program const program;
    std::vector<Option> const options;
    std::map<string, size_t> const opt_name_map;
    std::variant<std::vector<string>, std::vector<Command>> const
        args_or_subcommands;

    Option get_option(string const &word) const;
    Option get_option(char const &character) const;

    /**
     * @brief reduce args until the first element is not an option
     *
     * @param kwargs
     * @param args
     */
    void parse_options(
        std::map<string, std::any> kwargs, std::deque<string> args) const;

    void print_help() const;

  public:
    string const name;
    string parent_command = "";
    string const description; // help text
    int execute(
        std::deque<string> arguments, string group_name = "") const noexcept;

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
        std::vector<string> const arguments, string const name,
        string const description);
  };

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
