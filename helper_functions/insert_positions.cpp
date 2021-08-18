#include "Exception.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <vector>
using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

void sort_json_vectors_by_id(json &components);
void insert_positions(json &topology, json &second_json);

/** \brief Copies over positions from a topology json file (which only needs to
 * have "x","y" and "id" keys in each component.).
 *
 */
int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout
        << "You must provide a topology file, a positions and an outputfile"
        << std::endl;
    return 1;
  }
  std::string topfile = argv[1];
  std::string positionsfile = argv[2];
  std::string outfile = argv[3];

  json topology;
  {
    std::ifstream inputstream((fs::path(topfile)));
    inputstream >> topology;
  }

  json positions;
  {
    std::ifstream positionsstream(positionsfile);
    positionsstream >> positions;
  }

  sort_json_vectors_by_id(positions);
  sort_json_vectors_by_id(topology);

  insert_positions(topology, positions);

  {
    std::ofstream outstream(outfile);
    outstream << topology.dump(1, '\t');
  }
}

void insert_positions(json &topology, json &second_json) {
  for (auto const &component : {"nodes", "connections"}) {
    // only fire if the json contains entries of this component.
    if (not second_json.contains(component)) {
      continue;
    }
    for (auto &[key, ignored] : second_json[component].items()) {
      if (not topology[component].contains(key)) {
        std::cout << "Note: Topology json does not contain " << component
                  << " of type " << key << ", but the "
                  << "positions"
                  << " json does contain such " << component << "."
                  << std::endl;
        continue;
      }
      auto &second_json_vector_json = second_json[component][key];
      auto &topology_vector_json = topology[component][key];

      auto secjson_it = second_json_vector_json.begin();
      auto top_it = topology_vector_json.begin();

      // assign the additional values to the topology json.
      while (secjson_it != second_json_vector_json.end()
             and top_it != topology_vector_json.end()) {
        auto id_compare_less = [](json const &a, json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };

        if (id_compare_less(*secjson_it, *top_it)) {
          std::cout << "The component with id "
                    << (*secjson_it)["id"].get<std::string>() << '\n'
                    << " has an entry in the "
                    << "positions"
                    << " json but is not given in the "
                       "topology json."
                    << std::endl;
          ++secjson_it;
        } else if (id_compare_less(*top_it, *secjson_it)) {
          gthrow(
              {"The component with id ", (*top_it)["id"].get<std::string>(),
               "\n is given in the topology json but no ", "positions",
               " condition is provided."});

        } else {
          (*top_it)["x"] = (*secjson_it)["x"];
          (*top_it)["y"] = (*secjson_it)["y"];
          ++secjson_it;
          ++top_it;
        }
      }
    }
  }
}

void sort_json_vectors_by_id(json &components) {

  for (auto const &component_class : {"nodes", "connections"}) {
    if (not components.contains(component_class)) {
      continue;
    }
    for (auto &[component_type, sub_json] :
         components[component_class].items()) {
      auto &second_json_vector_json
          = components[component_class][component_type];
      // Define < function
      auto id_compare_less = [](json const &a, json const &b) -> bool {
        return a["id"].get<std::string>() < b["id"].get<std::string>();
      };
      std::sort(
          second_json_vector_json.begin(), second_json_vector_json.end(),
          id_compare_less);
    }
  }
}
