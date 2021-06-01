#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

/** @brief A helper function to move a component from one type to another
 *
 * The parameters are space-separated strings given as commandline options in
 * the following order.
 * @param from The exact name of the type the component is currently in
 * @param to The exact name of the type the component is to be moved to.
 * @param id the id of the component to be moved.
 */

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "You must provide an id and a resultsfile." << std::endl;
    return 1;
  }
  std::string from_category = "PVnode";
  std::string to_category = "Vphinode";
  std::string id = argv[1];
  std::string resultsfile = argv[2];

  std::vector<std::string> files
      = {"topology.json", "boundary.json", "initial.json", "control.json"};

  for (std::string const &file : files) {

    json input;
    {
      std::ifstream inputstream((fs::path(file)));
      inputstream >> input;
    }
    json results;
    if (file == "boundary.json") {

      std::ifstream resultsstream((fs::path(resultsfile)));
      resultsstream >> results;
    }
    bool found = false;
    for (auto &[compname, comptype] : input.items()) { // nodes and connections
      if (not comptype.contains(from_category)) {
        continue;
      }
      // define an equals function as a lambda
      auto id_equals = [id](nlohmann::json const &a) -> bool {
        return a["id"].get<std::string>() == id;
      };
      auto &currcat = comptype[from_category];
      auto it = std::find_if(currcat.begin(), currcat.end(), id_equals);
      if (it == currcat.end()) {
        continue;
      } else {
        auto current_json = *it;
        currcat.erase(it);
        if (not comptype.contains(to_category)) {
          comptype[to_category] = nlohmann::json::array();
        }
        comptype[to_category].push_back(current_json);
        found = true;

        if (file == "boundary.json") {
          json boundarydata = json::array();
          auto results_it = std::find_if(
              results[compname][from_category].begin(),
              results[compname][from_category].end(), id_equals);
          for (auto &timepoint : (*results_it)["data"]) {
            json boundary_timepoint;
            boundary_timepoint["time"] = timepoint["time"];
            boundary_timepoint["values"] = json::array();
            boundary_timepoint["values"].push_back(timepoint["V"]);
            boundary_timepoint["values"].push_back(timepoint["phi"]);
            boundarydata.push_back(boundary_timepoint);
          }
          comptype[to_category].back()["data"] = boundarydata;
        }
      }
    }
    if (not found) {
      std::cout << "No object with id " << id << " in category "
                << from_category << " in file " << file << std::endl;
    } else {
      {
        std::ofstream outputstream((fs::path(file)));
        outputstream << input.dump(1, '\t');
      }
    }
  }
}
