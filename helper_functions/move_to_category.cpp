#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

#include <iostream>
int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "You must provide a json file and the id of a component."
              << std::endl;
    return 1;
  }
  std::string id = argv[3];
  std::string from_category = argv[1];
  std::string to_category = argv[2];

  std::vector<std::string> files
      = {"topology.json", "boundary.json", "initial.json", "control.json"};

  for (auto const &file : files) {
    json input;
    std::fstream filestream((fs::path(file)));
    filestream >> input;

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
        comptype[to_category].push_back(*it);
        currcat.erase(it);
      }
    }
  }
}
