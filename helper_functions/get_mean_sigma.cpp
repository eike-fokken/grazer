#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, char **argv) {
  using json = nlohmann::json;
  std::string input_json_string = argv[1];
  std::string output_json_string = argv[2];

  auto inputfile = std::filesystem::path(input_json_string);
  auto outputfile = std::filesystem::path(output_json_string);

  json input;
  {
    std::ifstream inputstream(inputfile);
    inputstream >> input;
  }

  {
    std::ofstream outputstream(outputfile);
    outputstream << input;
  }
  std::cout << "first part worked" << std::endl;
}
