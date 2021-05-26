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
    std::cout << "You must provide a starting node id and an ending node id."
              << std::endl;
    return 1;
  }
  std::string from_category = "Vphinode";
  std::string to_category = "ExternalPowerplant";
  std::string startid = argv[1];
  std::string endid = argv[2];
  std::string fullstartid = "node_" + startid;
  std::string g2pid = "g2p_" + startid + "_" + endid;

  std::string g2pcategory = "Gaspowerconnection";

  std::vector<std::string> files
      = {"topology.json", "boundary.json", "initial.json", "control.json"};

  for (std::string const &file : files) {

    json input;
    {
      std::ifstream inputstream((fs::path(file)));
      inputstream >> input;
    }

    for (auto &comptype : input) { // nodes and connections
      if (not comptype.contains(g2pcategory)) {
        continue;
      }
      // define an equals function as a lambda
      auto id_equals = [g2pid](nlohmann::json const &a) -> bool {
        return a["id"].get<std::string>() == g2pid;
      };
      auto &currcat = comptype[g2pcategory];
      auto it = std::find_if(currcat.begin(), currcat.end(), id_equals);
      if (it == currcat.end()) {
        // not already present !;
        // here the magic happens!

        if (file == "control.json" or file == "boundary.json") {
          auto newg2p = currcat.front();
          currcat.insert(currcat.begin(), newg2p);
          currcat.front()["id"] = g2pid;
        } else if (file == "topology.json") {
          auto newg2p = currcat.front();
          currcat.insert(currcat.begin(), newg2p);
          currcat.front()["id"] = g2pid;
          currcat.front()["from"] = fullstartid;
          currcat.front()["to"] = endid;
        } else {
          auto newg2p = currcat.front();
          currcat.insert(currcat.begin(), newg2p);
          currcat.front()["id"] = g2pid;
        }

      } else {
        std::cout << "The gaspowerconnection is already there!" << std::endl;
        return 1;
      }
    }

    {
      std::ofstream outputstream((fs::path(file)));
      outputstream << input.dump(1, '\t');
    }
  }
}
