#include <Exception.hpp>
#include <Input_output.hpp>
#include <filesystem>
#include <iostream>

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

  Command::Command(
      io::program const _program, std::vector<Option> const _options,
      string const _name, string const _description) :
      program(_program),
      options(_options),
      name(_name),
      description(_description) {}

  void print_help(
      string program_name, string program_description,
      std::vector<Option> options, std::vector<Command> commands) {
    std::cout << "Usage: " << program_name << " "
              << (options.empty() ? "" : "[OPTIONS]")
              << (commands.empty() ? "" : "COMMAND") << "[ARGS]...\n";
    std::cout << "  " << program_description << "\n";
    if (not options.empty()) {
      std::cout << "Options:\n";
      for (auto const &option : options) {
        std::cout << "  " << option.name << "  " << option.description << "\n";
      }
      std::cout << "\n";
    } else if (not commands.empty()) {
      std::cout << "Commands:\n";
      for (auto const &command : commands) {
        std::cout << "  " << command.name << "  " << command.description
                  << "\n";
      }
      std::cout << std::endl;
    } else {
      gthrow({"Should not be reachable"});
    }
  }

  io::program program_switchboard(std::vector<Command> subcommands) {
    return
        [subcommands](
            std::deque<string> args, std::map<string, string> kwargs) -> int {
          if (not args.empty()) {
            for (auto const &command : subcommands) {
              if (command.name == args[0]) {
                args.pop_front();
                return command.execute(args, kwargs["__name__"] + command.name);
              }
            }
          }
          print_help(
              kwargs["__name__"], kwargs["__description__"],
              std::vector<Option>(), subcommands);
          return 1;
        };
  }

  Command::Command(
      std::vector<Command> const subcommands, string const _name,
      string const _description) :
      program(program_switchboard(subcommands)),
      options(std::vector<Option>()),
      name(_name),
      description(_description) {}

  std::deque<std::string> args_as_deque(int argc, char **argv) {
    return std::deque<std::string>(argv + 1, argv + argc);
  }

  int Command::execute(std::deque<string> arguments, string group_name) const {
    std::map<string, string> kwargs;
    kwargs["__name__"] = group_name + this->name;
    kwargs["__description__"] = this->description;
    // find command.options in arguments parse into kwargs, pop them
    std::deque<string> args = arguments;
    return this->program(args, kwargs);
  }

  std::filesystem::path prepare_output_dir(string output_dir_string) {

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
  extract_input_data(std::deque<std::string> const &cmd_arguments) {

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
          {"The given path\n\n",
           std::filesystem::absolute(problem_data_file).string(),
           "\n\ndoes not point to a regular file.  Maybe the name is "
           "misspelled."});
    }
    return problem_data_file;
  }

} // namespace io
