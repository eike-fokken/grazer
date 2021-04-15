#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

// void add_json_data(nlohmann::json const &input, nlohmann::json &output);
// void add_json_data(nlohmann::json const &input, nlohmann::json &output) {

//   for (auto &componenttype : input) {
//     for (auto &component : componenttype) {}
//   }
// }

int main(int argc, char **argv) {

  std::string output_dir_trunk = argv[1];

  for (auto &pit : fs::directory_iterator(fs::current_path())) {
    // check only directories:
    if (not fs::is_directory(pit.path())) {
      continue;
    }
    // ignore directories starting with . (hidden files in unix)
    if ((pit.path().filename().string()).find(".") == 0) {
      continue;
    }
    // auto current_dirname = pit.path().filename();
    // std::cout << pit.path().filename() << std::endl;

    if ((pit.path().filename().string()).find(output_dir_trunk) == 0) {
      std::cout << pit.path().filename() << std::endl;
    }
  }

  // json input;
  // {
  //   std::ifstream inputstream(inputfile);
  //   inputstream >> input;
  // }

  // json output;

  // {
  //   std::ofstream outputstream(outputfile);
  //   outputstream << input;
  // }
  // std::cout << "first part worked" << std::endl;
}
