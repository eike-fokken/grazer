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
  double influx;
  std::filesystem::path inputfile = "problem/boundary.json";
  if (argc != 2) {
    std::cout << "You must provide a total influx number." << std::endl;
    return 1;
  } else {
    influx = std::stod(argv[1]);
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

  double old_summed_input = 0;
  try {
    for (auto const &source : results["nodes"]["Source"]) {
      if (source["data"][0]["values"][0].get<double>()
          != source["data"][1]["values"][0].get<double>()) {
        throw std::runtime_error("old gas input was not constant!");
      }
      old_summed_input += source["data"][0]["values"][0].get<double>();
    }
  } catch (std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown error, aborting!" << std::endl;
    return 1;
  }

  try {
    for (auto &source : results["nodes"]["Source"]) {
      source["data"][0]["values"][0]
          = source["data"][0]["values"][0].get<double>() * influx
            / old_summed_input;
      source["data"][1]["values"][0]
          = source["data"][1]["values"][0].get<double>() * influx
            / old_summed_input;
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
