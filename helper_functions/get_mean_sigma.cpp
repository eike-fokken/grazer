#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

void add_json_data(nlohmann::json const &input, nlohmann::json &output);
void add_json_data(nlohmann::json const &input, nlohmann::json &output) {

  for (auto &componenttype : input) {
    for (auto &component : componenttype) {}
  }
}

int main(int argc, char **argv) {

  std::string output_dir_trunk;
  if (argc >= 2) {
    output_dir_trunk = argv[1];
  } else {
    output_dir_trunk = "output";
  }
  std::cout << output_dir_trunk << std::endl;

  std::string computed_output;
  if (argc >= 3) {
    computed_output = argv[2];
  } else {
    computed_output = "mean_and_sigma.json";
  }
  std::cout << computed_output << std::endl;

  json output;

  for (auto &pit : fs::directory_iterator(fs::current_path())) {
    // check only directories:
    if (not fs::is_directory(pit.path())) {
      continue;
    }
    // ignore directories starting with . (hidden files in unix)
    if ((pit.path().filename().string()).find(".") == 0) {
      continue;
    }
    if ((pit.path().filename().string()).find(output_dir_trunk) == 0) {
      std::cout << "Processing " << pit.path().filename() << std::endl;

      json input;
      {
        fs::path currpath
            = fs::absolute(pit.path()) / fs::path("output.json").string();
        std::ifstream inputstream(currpath);
        inputstream >> input;
      }
      add_json_data(input, output);
    }
  }

  // {
  //   std::ofstream outputstream(outputfile);
  //   outputstream << input;
  // }
  // std::cout << "first part worked" << std::endl;
}
