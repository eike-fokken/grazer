#include "Input_output.hpp"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

/** \brief Outputs data from the controls.json of grazer to a csv file.
 */

int main(int argc, char **argv) {

  if (argc != 3) {
    std::cout
        << "You must provide a controls.json file and the id of a component."
        << std::endl;
    return 1;
  }
  std::string datafile = argv[1];

  std::cout << "input file is: " << datafile << std::endl;
  std::string id = argv[2];
  std::cout << "Component id: " << id << std::endl;

  json input;
  {
    std::ifstream inputstream(datafile);
    inputstream >> input;
  }

  std::vector<std::string> controltypes = {"Controlvalve", "Compressorstation"};

  json id_object;
  id_object["id"] = id;
  auto id_compare_less
      = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
    return a["id"].get<std::string>() < b["id"].get<std::string>();
  };
  bool object_is_found = false;
  json object;
  std::vector componentclass{"nodes", "connections"};
  for (auto const &compclass : componentclass) {
    if (not input.contains(compclass)) {
      continue;
    }
    for (auto &[component_type, component_array] : input[compclass].items()) {
      auto it = std::lower_bound(
          component_array.begin(), component_array.end(), id_object,
          id_compare_less);
      if (it != component_array.end()) {
        if ((*it)["id"] != id) {
          continue;
        } else {
          object = (*it);
          auto control_type = std::find(
              controltypes.begin(), controltypes.end(), component_type);

          if (control_type != controltypes.end()) {
            object_is_found = true;
          } else {
            std::cout
                << __FILE__ << ":" << __LINE__ << ":\n"
                << "The required id is not a gas type and "
                   "not a power type.\n"
                << "Probably Grazer incorporates more types and this "
                   "helper function should be fixed to incorporate the new "
                   "types\n.  Aborting now...";
            return 1;
          }
          break;
        }
      }
    }
  }
  if (object.empty()) {
    std::cout << "Didn't find object with id: " << id << std::endl;
    return 1;
  }

  try {
    // here write the csv!
    static_assert(false, "hier weiter!");

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    std::cout << "Aborting..." << std::endl;
    return 1;
  }
}
