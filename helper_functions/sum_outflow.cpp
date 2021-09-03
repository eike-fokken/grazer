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

/** \brief Prints the summed outflux of a network.
 *
 * Best used in conjuction with change_inflow.
 */

int main(int argc, char **argv) {
  std::filesystem::path inputfile;
  std::filesystem::path outputfile;
  if (argc != 2) {
    std::cout << "You must provide a results json file" << std::endl;
    return 1;
  } else {
    inputfile = argv[1];
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

  double summed_output = 0;
  try {
    for (auto const &shortpipe : results["connections"]["Shortpipe"]) {
      // if (shortpipe["data"][1]["values"][1].get<double>() < 0) {
      //   std::cout << "Negative!" << std::endl;
      // }
      summed_output += shortpipe["data"][1]["values"][1].get<double>();
    }
  } catch (std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown error, aborting!" << std::endl;
    return 1;
  }
  std::cout << summed_output << std::endl;
  return 0;
}
