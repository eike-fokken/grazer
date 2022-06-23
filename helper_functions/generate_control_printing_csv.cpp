#include "Input_output.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

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
          auto type_iterator = std::find(
              controltypes.begin(), controltypes.end(), component_type);

          if (type_iterator == controltypes.end()) {
            std::cout
                << __FILE__ << ":" << __LINE__ << ":\n"
                << "The required object id does not belong to an object of "
                   "Controlcomponent type.\n"
                << "Probably Grazer incorporates now more Controlcomponent "
                   "types and this "
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
    fs::path outpath(id + ".csv");
    if (not io::absolute_file_path_in_root(fs::current_path(), outpath)) {
      throw std::invalid_argument(
        "the path " + outpath.string()
        + " is not in the current working directory!\nProbably the id is wrong.");
    }

    std::cout << "Creating or overwriting " << outpath.string() << std::endl;
    std::ofstream outstream(outpath);

    outstream << "time";
    if (not object["data"][0]["values"].is_array()) {
      std::cout << "values is not an array, something is wrong with the "
                   "control json. Aborting..."
                << std::endl;
      fs::remove(outpath);
      return 1;
    }
    for (size_t i = 0; i != object["data"][0]["values"].size(); ++i) {
      outstream << ",   "
                << "control_" << std::to_string(i);
    }
    outstream << "\n";

    for (auto &timestep : object["data"]) {
      outstream << timestep["time"];
      for (auto const &value : timestep["values"]) {
        if (not value.is_number()) {
          std::cout << std::endl;
          fs::remove(outpath);
          std::cout << "a value inside the values vector is not a number, "
                       "something is wrong with the "
                       "control json. Aborting..."
                    << std::endl;
          return 1;
        }
        outstream << ",   " << value;
      }
      outstream << "\n";
    }

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    std::cout << "Aborting..." << std::endl;
    return 1;
  }
}
