#include <filesystem>
#include <string>
#include <tuple>
#include <vector>


namespace Aux_executable {

  std::filesystem::path prepare_output_dir(std::string output_dir);

  std::tuple<std::filesystem::path, std::filesystem::path,
             std::filesystem::path, double, double, double>
  extract_input_data(std::vector<std::string> cmd_arguments);

  std::tuple<std::filesystem::path, std::filesystem::path,
             std::filesystem::path>
  prepare_inputfiles(std::vector< std::string > input_filenames);

}

