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

  Option::Option(
      string const _name, uint16_t const _nargs,
      std::vector<string> const _alias, string const _description,
      std::optional<std::any> const _default_value,
      std::function<std::any(std::vector<string>)> const _parser,
      std::optional<
          std::function<int(Command const *, Option const *, std::any)>> const
          _callback,
      bool const _is_eager) :
      default_value(_default_value),
      name(_name),
      description(_description),
      alias(_alias),
      nargs(_nargs),
      is_eager(_is_eager),
      parser(_parser),
      callback(_callback) {}

  std::vector<Option> preprocess_options(std::vector<Option> options);
  std::vector<Option> preprocess_options(std::vector<Option> options) {
    options.push_back(Option(
        "help", 0, {"h"}, "display this help text", std::nullopt,
        [](std::vector<string>) { return std::any(std::nullopt); },
        [](Command const *command, Option const * /*option*/, std::any) {
          command->print_help();
          return 0;
        },
        true));
    return options;
  }

  const char *EagerOptionEncountered::what() const noexcept {
    return "encountered eager option with return code: " + this->exit_code;
  }

  EagerOptionEncountered::EagerOptionEncountered(int _exit_code) :
      exit_code(_exit_code) {}

  void print_commands(std::vector<Command> commands);
  void print_commands(std::vector<Command> commands) {
    if (not commands.empty()) {
      size_t max_name_size = 0;
      for (auto const &cmd : commands) {
        size_t cmd_size = cmd.name.size();
        max_name_size = cmd_size > max_name_size ? cmd_size : max_name_size;
      }
      std::cout << "Commands:\n";
      for (auto const &command : commands) {
        size_t padding = max_name_size - command.name.size();
        std::cout << "  " << command.name << "  " << string(padding, ' ')
                  << command.description << "\n";
      }
      std::cout << "\n";
    }
  }

  std::optional<string> find_character_alias(Option option);
  std::optional<string> find_character_alias(Option option) {
    for (auto const &alias : option.alias) {
      if (alias.size() == 1) {
        return alias;
      }
    }
    return std::nullopt;
  }

  void print_options(std::vector<Option> options);
  void print_options(std::vector<Option> options) {
    if (not options.empty()) {
      size_t max_name_size = 0;
      for (auto const &option : options) {
        size_t opt_size = option.name.size();
        max_name_size = opt_size > max_name_size ? opt_size : max_name_size;
      }
      std::cout << "Options:\n";
      for (auto const &option : options) {
        auto char_alias = find_character_alias(option);
        std::cout << "  ";
        if (char_alias.has_value()) {
          std::cout << "-" << char_alias.value() << ", ";
        } else {
          std::cout << "    "; // 4 (1. dash, 2. letter, 3. comma, 4. space)
        }
        size_t padding = max_name_size - option.name.size();
        std::cout << "--" << option.name << "  " << string(padding, ' ')
                  << option.description << "\n";
      }
      std::cout << "\n";
    }
  }

  void Command::print_help() const {
    // print first line
    std::cout << "\nUsage: "
              << (this->parent_command.has_value()
                      ? parent_command.value()->name + " "
                      : "")
              << this->name << " "
              << (this->options.empty() ? "" : "[OPTIONS] ");
    bool command_group = std::holds_alternative<std::vector<Command>>(
        this->args_or_subcommands);
    if (command_group) {
      std::cout << "COMMAND [ARGS]...\n";
    } else {
      auto arguments = std::get<std::vector<string>>(args_or_subcommands);
      for (string arg : arguments) {
        for (auto &c : arg) c = static_cast<char>(toupper(c));
        std::cout << arg << "\n";
      }
    }
    // print description
    std::cout << (this->description == "" ? "" : "\n   " + description + "\n")
              << "\n";
    // print options
    print_options(this->options);
    // print commands
    if (command_group) {
      auto commands = std::get<std::vector<Command>>(this->args_or_subcommands);
      print_commands(commands);
    }
    std::cout << std::endl;
  }

  template <typename K, typename E>
  void insert_new(std::map<K, E> &map, K key, E element) {
    const auto [it, success] = map.insert({key, element});
    if (not success) {
      gthrow({"The element to insert is not new!"});
    }
    return;
  }

  std::map<string, size_t> hash_map_from_vec(std::vector<Option> options);
  std::map<string, size_t> hash_map_from_vec(std::vector<Option> options) {
    std::map<string, size_t> result;
    for (size_t idx = 0; idx < options.size(); idx++) {
      insert_new(result, options[idx].name, idx);
      for (auto const &alias : options[idx].alias) {
        insert_new(result, alias, idx);
      }
    }
    return result;
  }

  io::program group(Command *parent, std::vector<Command> subcommands) {
    for (auto &subcommand : subcommands) { subcommand.parent_command = parent; }
    return [subcommands](
               std::deque<string> args,
               std::map<string, std::any> /*kwargs*/) -> int {
      if (not args.empty()) {
        for (auto const &command : subcommands) {
          if (command.name == args[0]) {
            args.pop_front();
            return command(args);
          }
        }
        throw std::invalid_argument("No such command: " + args[0]);
      }
      throw std::invalid_argument("Missing Command");
    };
  }

  Command::Command(
      io::program const _program, std::vector<Option> _options,
      std::vector<string> arguments, string const _name,
      string const _description) :
      script(_program),
      options(preprocess_options(_options)),
      opt_name_map(hash_map_from_vec(options)),
      args_or_subcommands(arguments),
      name(_name),
      description(_description) {}

  Command::Command(
      std::vector<Command> const subcommands, string const _name,
      string const _description) :
      script(group(this, subcommands)),
      options(preprocess_options(std::vector<Option>())),
      opt_name_map(hash_map_from_vec(options)),
      args_or_subcommands(subcommands),
      name(_name),
      description(_description) {}

  std::deque<std::string> args_as_deque(int argc, char **argv) {
    return std::deque<std::string>(argv + 1, argv + argc);
  }

  int Command::operator()(std::deque<string> args) const noexcept {
    std::map<string, std::any> kwargs;
    try {
      this->parse_options(kwargs, args);
      return this->script(args, kwargs);
    } catch (EagerOptionEncountered const &ex) {
      return ex.exit_code;
    } catch (std::invalid_argument const &ex) {
      std::cout << "[Invalid Argument]: " << ex.what() << std::endl;
      this->print_help();
    } catch (std::exception const &ex) {
      std::cout << "[Error]: " << ex.what() << std::endl;
    } catch (...) {
      std::cout << "An unkown type of exception was thrown.\n\n"
                   "This is a bug and must be fixed!\n\n"
                << std::endl;
    }
    return 1;
  }

  Option Command::get_option(string const &word) const {
    auto search = this->opt_name_map.find(word);
    if (search == opt_name_map.end()) {
      throw std::invalid_argument(word);
    }
    // opt_name_map was only filled with valid indices this should *never* be
    // out of range
    return this->options[search->second];
  }

  Option Command::get_option(char const &character) const {
    return get_option(string(1, character));
  }

  void Command::parse_options(
      std::map<string, std::any> kwargs, std::deque<string> args) const {
    if (args.empty()) {
      return;
    }
    string option_str = args.front();
    if (option_str.rfind("-", /*pos=*/0) == string::npos) {
      return; // no more options
    }
    if (option_str.size() < 2) {
      throw std::invalid_argument("Single dash '-' as argument");
      // not sure how to handle it
    }
    // definitely have an option: pop args
    args.pop_front();
    if (option_str[1] == '-') {
      //--option
      if (option_str.size() < 3) {
        return; // -- indicates all following words are arguments not options
      }
      auto eq_pos = option_str.find("=");
      Option option = this->get_option(option_str.substr(2, eq_pos));

      std::vector<string> arguments;
      if (eq_pos == string::npos) { // --option [value]
        arguments.reserve(option.nargs);
        for (int idx = 0; idx < option.nargs; idx++) {
          arguments.push_back(args.front());
          args.pop_front();
        }
      } else { // --option=value
        if (option.nargs != 1) {
          throw std::invalid_argument(
              "how is an --option=value with more/less values than 1 supposed "
              "to look?");
        }
        arguments.push_back(option_str.substr(eq_pos + 1));
      }
      kwargs[option.name] = option.parser(arguments);
      if (option.callback.has_value()) {
        int exit_code
            = option.callback.value()(this, &option, kwargs[option.name]);
        if (option.is_eager) {
          throw EagerOptionEncountered(exit_code);
        }
      }
      return this->parse_options(kwargs, args);
    }
    // character options
    if (option_str.size() > 2 and option_str[2] != '=') {
      // concatenated flag options -oaf where o a and f are all options
      for (char const c : option_str.substr(1)) {
        Option option = this->get_option(c);
        if (not option.nargs == 0) {
          throw std::invalid_argument(
              "concatenated option " + string(1, c) + " is not a flag");
        }
        kwargs[option.name] = option.parser(std::vector<string>());
        if (option.callback.has_value()) {
          int exit_code
              = option.callback.value()(this, &option, kwargs[option.name]);
          if (option.is_eager) {
            throw EagerOptionEncountered(exit_code);
          }
        }
      }
      return this->parse_options(kwargs, args);
    }
    Option option = this->get_option(option_str[1]);
    std::vector<string> arguments;

    if (option_str.size() <= 2) {
      arguments.reserve(option.nargs);
      for (int idx = 0; idx < option.nargs; idx++) {
        arguments.push_back(args.front());
        args.pop_front();
      }
    } else if (option_str[2] == '=') {
      if (option.nargs != 1) {
        throw std::invalid_argument(
            "how is an --option=value with more/less values than 1 supposed "
            "to look?");
      }
      arguments.push_back(option_str.substr(3));
    } else {
      throw std::logic_error("unreachable statement");
    }
    kwargs[option.name] = option.parser(arguments);
    if (option.callback.has_value()) {
      int exit_code
          = option.callback.value()(this, &option, kwargs[option.name]);
      if (option.is_eager) {
        throw EagerOptionEncountered(exit_code);
      }
    }
    return this->parse_options(kwargs, args);
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
