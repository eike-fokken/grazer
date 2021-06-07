#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
  double pressure_diff = 0;
  std::filesystem::path inputfile;
  if (argc != 3) {
    std::cout << "You must provide a problem folder and a pressure difference "
                 "to be added to the "
                 "existing pressures."
              << std::endl;
    return 1;
  } else {
    inputfile = std::string(argv[1]) + "/problem/initial.json";
    pressure_diff = std::stod(argv[2]);
  }

  if (not fs::exists(inputfile)) {
    std::cout << "The input file doesn't exist!" << std::endl;
    return 1;
  }

  json results;
  {
    std::ifstream resultsstream(inputfile);
    resultsstream >> results;
  }

  std::vector<std::string> gastypes
      = {"Pipe", "Controlvalve", "Compressorstation", "Shortpipe",
         "Gaspowerconnection"};
  try {
    for (auto const &type : gastypes) {
      if (not results["connections"].contains(type)) {
        continue;
      }
      for (auto &gastype : results["connections"][type]) {
        for (auto &datapoint : gastype["data"]) {
          double value = datapoint["values"][0].get<double>();
          value += pressure_diff;
          datapoint["values"][0] = value;
        }
      }
    }
  } catch (std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown error, aborting!" << std::endl;
    return 1;
  }

  {
    std::ofstream resultsstream(inputfile);
    resultsstream << results.dump(1, '\t');
  }

  return 0;
}
