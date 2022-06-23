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

/** \brief Writes the seeds used in an output file into the boundary values of a
 * problem folder.
 *
 * Used to reproduce an output file that had stochastic inputs.
 * - First argument is the problem folder
 * - Second argument is the states output json file.
 */

int main(int argc, char **argv) {
  fs::path problemfolder;
  fs::path resultsfile;
  if (argc != 3) {
    std::cout << "You must provide a problem folder and a results json file "
                 "(usually called \"output.json\")."
              << std::endl;
    return 1;
  } else {

    problemfolder = std::string(argv[1]);
    resultsfile = std::string(argv[2]);
  }

  if (not fs::exists(resultsfile)) {
    std::cout << "The file " << resultsfile.string() << " does not exist!"
              << std::endl;
    return 1;
  }

  fs::path boundaryfile = problemfolder / fs::path("problem/boundary.json");
  if (not fs::exists(boundaryfile)) {
    std::cout << "The file " << boundaryfile.string() << " does not exist!"
              << std::endl;
    return 1;
  }

  json results;
  {
    std::ifstream resultsstream(resultsfile);
    resultsstream >> results;
  }

  json boundary;
  {
    std::ifstream boundarystream(boundaryfile);
    boundarystream >> boundary;
  }

  std::vector<std::string> stochastictypes = {"StochasticPQnode"};
  for (auto const &componentclass : {"nodes", "connections"}) {
    if (not results.contains(componentclass)) {
      continue;
    }
    for (auto const &type : stochastictypes) {
      if (not results[componentclass].contains(type)) {
        continue;
      }
      for (auto const &component : results[componentclass][type]) {
        if (component.contains("seed")) {
          auto id = component["id"].get<std::string>();

          auto find_id = [&](json x) { return id == x["id"]; };
          auto iterator = std::find_if(
              boundary[componentclass][type].begin(),
              boundary[componentclass][type].end(), find_id);

          if (iterator != boundary[componentclass][type].end()) {
            (*iterator)["seed"] = component["seed"];
          } else {
            std::cout << "No component with id " << id
                      << " was found in the boundary file.\n"
                      << "Aborting..." << std::endl;
            return 1;
          }

        } else {
          std::cout << "There was no seed in the output file for "
                    << component["id"] << "Aborting!" << std::endl;
          return 1;
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
