#include "Input_output.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, char **argv) {

  if (argc != 3) {
    std::cout << "You must provide a json file and the id of a powernode."
              << std::endl;
    return 1;
  }
  std::string datafile = argv[1];

  std::cout << "input file is: " << datafile << std::endl;
  std::string id = argv[2];
  std::cout << "Powernode id: " << id << std::endl;

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

  fs::path outpath(id + ".csv");
  if (not Aux_executable::absolute_file_path_in_root(
          fs::current_path(), outpath)) {
    std::cout << "the path " << outpath.string()
              << " is not in the current working directory!\n"
              << "Probably the id is wrong."
              << "Aborting...";
    return 2;
  }
  std::ofstream outstream(outpath);
  outstream << "time";
  for (auto q_it = object["data"][0].begin(); q_it != object["data"][0].end();
       ++q_it) {
    if (q_it.key() == "time") {
      continue;
    }
    outstream << ",   " << q_it.key();
  }
  outstream << "\n";

  for (auto &timestep : object["data"]) {
    outstream << timestep["time"];
    for (auto q_it = timestep.begin(); q_it != timestep.end(); ++q_it) {
      if (q_it.key() == "time") {
        continue;
      }
      if (not q_it.value().is_number()) {
        std::cout << "This seems to be no powernode, as the value of "
                  << q_it.key() << " is not just a number.\n Aborting..."
                  << std::endl;
        fs::remove(outpath);
        return 1;
      }
      outstream << ",   " << q_it.value();
    }
    outstream << "\n";
  }
}
