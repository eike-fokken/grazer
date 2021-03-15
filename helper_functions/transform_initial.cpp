#include <filesystem>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cout
        << "You must specify exactly one topology file, the initial_value file "
           "and one output file for the new initial value format"
           "on the command line!"
        << std::endl;
    return 1;
  }

  if (std::filesystem::exists(argv[3])) {
    std::cout << "The output file exists, overwrite?\n";
    std::string answer;
    std::cin >> answer;

    if (answer == "n" or answer == "no") {
      std::cout << "Ok, I won't overwrite and exit now." << std::endl;
      return 0;
    }

    if (answer != "y" and answer != "yes") {
      std::cout << "You must answer yes/no or y/n, aborting!" << std::endl;
      return 1;
    }
  }

  nlohmann::ordered_json topology;
  nlohmann::ordered_json initialvalues;
  nlohmann::ordered_json new_initialvalues;
  {
    std::ifstream topology_filestream(argv[1]);
    topology_filestream >> topology;
  }

  {
    std::ifstream initialvalues_filestream(argv[2]);
    initialvalues_filestream >> initialvalues;
  }

  std::map<std::string,bool> initial_values_present = {{"nodes", false},{"connections",false}};
  auto const & initial_json_vector = initialvalues["initialvalues"];
  for (auto type : {"nodes", "connections"}) {
    if (topology.contains(type)) {
      for (auto it = topology[type].begin(); it != topology[type].end();++it
           // auto &componenttype : topology[type]
      ) {
        auto componenttypename = it.key();
        auto componenttypevector = it.value();
        for (auto &componentjson : componenttypevector) {
          std::string const & component_id = componentjson["id"];
          auto finder = [component_id](nlohmann::ordered_json const &x) {
            auto itr = x.find("id");
            return itr != x.end() and itr.value() == component_id;
          };
          auto initjson = std::find_if(initial_json_vector.begin(),
                                       initial_json_vector.end(), finder);
          if (initjson == initial_json_vector.end()) {
            std::cout << " object " << component_id
                      << " has no initial condition." << std::endl;
          } else {
            // if(not initial_values_present[type]){
            //   initial_values_present[type] = true;
            // }
            // if()
            new_initialvalues[type][componenttypename].push_back(*initjson);
          }
        }
      }
    }
  }

  std::ofstream out(argv[3]);
  out << std::setw(4) << new_initialvalues << std::endl;
}
