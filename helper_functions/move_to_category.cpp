#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

/** @brief A helper function to move a component from one type to another.
 *
 * The parameters are space-separated strings given as commandline options in
 * the following order.
 *
 * The function expects the following command line arguments:
 *
 * - the exact name of the type the component is currently in
 * - the exact name of the type the component is to be moved to
 * - the id of the component to be moved.
 *
 */

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout << "You must provide a from category, a to category and an id."
              << std::endl;
    return 1;
  }
  std::string from_category = argv[1];
  std::string to_category = argv[2];
  std::string id = argv[3];

  std::vector<std::string> files
      = {"topology.json", "boundary.json", "initial.json", "control.json"};

  for (std::string const &file : files) {

    json input;
    {
      std::ifstream inputstream((fs::path(file)));
      inputstream >> input;
    }
    bool found = false;
    for (auto &comptype : input) { // nodes and connections
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
