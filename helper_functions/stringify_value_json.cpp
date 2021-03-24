#include "Misc.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "You must specify exactly one input file and one output file"
                 "on the command line!"
              << std::endl;
    return 1;
  }

  if (std::filesystem::exists(argv[2])) {
    std::cout << "The output files exists, overwrite?\n";
    std::string answer;
    std::cin >> answer;

    if (answer == "n" or answer == "no") {
      std::cout << "Ok, I won't overwrite and exit now." << std::endl;
      return 0;
    }

    if (answer != "y" and answer != "yes") {
      std::cout << "You must answer yes/no or y/n, aborting!" << std::endl;
      return 1;
    }
  }

  nlohmann::ordered_json value_json;

  {
    std::ifstream value_filestream(argv[1]);
    value_filestream >> value_json;
  }

  for (auto type : {"nodes", "connections"}) {
    if (not value_json.contains(type)) {
      continue;
    }
    for (auto &componenttype : value_json[type]) {

      for (auto &componentjson : componenttype) {
        for (auto &datapoint : componentjson["data"]) {
          std::vector<double> vec;
          try {
            vec = datapoint["values"].get<std::vector<double>>();
          } catch (...) {
            std::cout << componentjson["id"] << std::endl;
            throw;
          }
          std::vector<std::string> string_vec;
          for (auto value : vec) {
            string_vec.push_back(Aux::to_string_precise(value));
          }
          datapoint["values"] = string_vec;
        }
      }
    }
  }
  std::ofstream out(argv[2]);
  out << std::setw(4) << value_json << std::endl;
}
