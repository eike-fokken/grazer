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
/**
 * Removes all seed entries from boundary.json file.
 */
int main(int argc, char **argv) {
  fs::path problemfolder;
  if (argc != 2) {
    std::cout << "You must provide a problem folder." << std::endl;
    return 1;
  } else {

    problemfolder = std::string(argv[1]);
  }

  fs::path boundaryfile = problemfolder / fs::path("problem/boundary.json");
  if (not fs::exists(boundaryfile)) {
    std::cout << "The file " << boundaryfile.string() << " does not exist!"
              << std::endl;
    return 1;
  }

  json boundary;
  {
    std::ifstream boundarystream(boundaryfile);
    boundarystream >> boundary;
  }

  std::vector<std::string> stochastictypes = {"StochasticPQnode"};
  for (auto const &componentclass : {"nodes", "connections"}) {
    if (not boundary.contains(componentclass)) {
      continue;
    }
    for (auto &type : boundary[componentclass]) {
      for (auto &component : type) {
        if (component.contains("seed")) {
          component.erase("seed");
        }
      }
    }
  }
  {
    std::ofstream boundarystream(boundaryfile);
    boundarystream << boundary.dump(1, '\t');
  }

  return 0;
}
