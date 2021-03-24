#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  using json = nlohmann::ordered_json;
  if (argc != 3) {
    std::cout << "You must provide the file to be transformed and the filename "
                 "of the output file in that order."
              << std::endl;
    return 1;
  }

  if (std::filesystem::exists(argv[2])) {
    std::cout << "The output files exists, overwrite?" << std::endl;
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

  json oldboundary;
  json newboundary;

  std::vector<std::string> nodetypes
      = {"Vphinode", "PVnode", "PQnode", "Flowboundarynode"};
  for (auto const &type : nodetypes) {
    newboundary["nodes"][type] = json::array();
  }
  {
    std::ifstream boundary_filestream(argv[1]);
    boundary_filestream >> oldboundary;
  }

  for (auto condition : oldboundary["boundarycondition"]) {
    for (auto const &type : nodetypes) {
      if (condition["type"] == type) {
        condition.erase("type");
        newboundary["nodes"][type].push_back(condition);
      }
    }
  }

  std::ofstream boundary_ofstream(argv[2]);
  boundary_ofstream << std::setw(4) << newboundary << std::endl;
}
