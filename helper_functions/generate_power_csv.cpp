#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
  std::string datafile = "mean_and_sigma.json";
  std::cout << "input file is: " << datafile << std::endl;

  if (argc <= 2) {
    std::cout << "\nYou must provide an id and quantities to include!\n"
              << std::endl;
    return 1;
  }
  std::string id = argv[1];

  std::vector<std::string> quantities;
  for (auto iargs = 2; iargs != argc; ++iargs) {
    quantities.push_back(argv[iargs]);
  }

  json input;
  {
    std::ifstream inputstream(datafile);
    inputstream >> input;
  }

  json id_object;
  id_object["id"] = id;
  auto id_compare_less
      = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
    return a["id"].get<std::string>() < b["id"].get<std::string>();
  };

  json object;
  for (auto &type : input) {
    auto it = std::lower_bound(
        type.begin(), type.end(), id_object, id_compare_less);
    if (it != type.end()) {
      if ((*it)["id"] != id) {
        continue;
      } else {
        object = (*it);
        break;
      }
    }
  }
  if (object.empty()) {
    std::cout << "Didn't find object with id: " << id << std::endl;
    return 1;
  }

  for (auto quantity_it = quantities.begin();
       quantity_it != quantities.end();) {
    if (not object["data"][0].contains(*quantity_it)) {
      std::cout << "Object with id " << id
                << " doesn't contain a quantity named\n"
                << (*quantity_it) << "\nskipping..." << std::endl;
      quantities.erase(quantity_it);
    } else {
      ++quantity_it;
    }
  }
  if (quantities.empty()) {
    std::cout << "No quantities, that are present in object with id " << id
              << " were requested.\n";
    return 1;
  }

  fs::path outpath(id + ".csv");
  std::ofstream outstream(outpath);
  outstream << "t,   ";
  for (auto quantity : quantities) { outstream << quantity << ",   "; }
  outstream << "\n";

  for (auto &timestep : object["data"]) {
    outstream << timestep["time"] << ",   ";
    for (auto &quantity : quantities) {
      outstream << timestep[quantity] << ",   ";
    }
    outstream << "\n";
  }
}
