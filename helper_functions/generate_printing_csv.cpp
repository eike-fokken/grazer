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

void write_gas_csv(std::string id, json object);
void write_power_csv(std::string id, json object);

/** \brief Outputs data from the output.json of grazer to a csv file.
 *
 * This is especially useful for printing with LaTeX.
 * Takes two arguments: First the json file and second a component id, whose
 * values should be written to the csv file.
 * For power nodes a single file name <id>.csv is created. If the file is
 * already there it is silently overwritten.
 * For gas edges multiple files are created, one for each quantity, named
 * <id>_<quantity>.csv.
 * For example, if "N1" is a powernode id in output.json
 * generate_printing_csv output.json N1
 * will write all data of N1 to N1.csv.
 * If "pipeline1" is a Pipe id,
 * generate_printing_csv output.json pipeline1
 * will create files pipeline1_pressure.csv and pipeline1_flow.csv.
 */

int main(int argc, char **argv) {

  if (argc != 3) {
    std::cout << "You must provide a json file and the id of a component."
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

  std::vector<std::string> gastypes
      = {"Pipe", "Controlvalve", "Compressorstation", "Shortpipe",
         "Gaspowerconnection"};
  std::vector<std::string> powertypes = {
      "Vphinode", "PQnode", "PVnode", "StochasticPQnode", "ExternalPowerplant"};
  json id_object;
  id_object["id"] = id;
  auto id_compare_less
      = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
    return a["id"].get<std::string>() < b["id"].get<std::string>();
  };

  json object;
  bool is_gas = false;
  bool is_power = false;
  for (auto type_it = input.begin(); type_it != input.end(); ++type_it) {
    auto it = std::lower_bound(
        type_it.value().begin(), type_it.value().end(), id_object,
        id_compare_less);
    if (it != type_it.value().end()) {
      if ((*it)["id"] != id) {
        continue;
      } else {
        object = (*it);
        auto gas_type
            = std::find(gastypes.begin(), gastypes.end(), type_it.key());
        auto power_type
            = std::find(powertypes.begin(), powertypes.end(), type_it.key());

        if (gas_type != gastypes.end()) {
          is_gas = true;
        } else if (power_type != powertypes.end()) {
          is_power = true;
        } else {
          std::cout << __FILE__ << ":" << __LINE__ << ":\n"
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

  if (object.empty()) {
    std::cout << "Didn't find object with id: " << id << std::endl;
    return 1;
  }

  try {
    if (is_gas) {
      write_gas_csv(id, object);
    } else if (is_power) {
      write_power_csv(id, object);
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    std::cout << "Aborting..." << std::endl;
    return 1;
  }
}

void write_power_csv(std::string id, json object) {
  fs::path outpath(id + ".csv");
  if (not Aux_executable::absolute_file_path_in_root(
          fs::current_path(), outpath)) {
    throw std::invalid_argument(
        "the path " + outpath.string()
        + " is not in the current working directory!\nProbably the id is wrong.");
  }

  std::cout << "Creating or overwriting " << outpath.string() << std::endl;
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
        std::cout << std::endl;
        fs::remove(outpath);
        throw std::invalid_argument(
            "This seems to be no powernode, as the value of " + q_it.key()
            + " is not just a number.");
      }
      outstream << ",   " << q_it.value();
    }
    outstream << "\n";
  }
}

void write_gas_csv(std::string id, json object) {
  auto first_step = object["data"][0];

  for (auto q_it = first_step.begin(); q_it != first_step.end(); ++q_it) {
    if (q_it.key() == "time") {
      continue;
    }
    fs::path outpath(id + "_" + q_it.key() + ".csv");
    std::cout << "Creating or overwriting " << outpath.string() << std::endl;

    if (not Aux_executable::absolute_file_path_in_root(
            fs::current_path(), outpath)) {
      throw std::invalid_argument(
        "the path " + outpath.string()
        + " is not in the current working directory!\nProbably the id is wrong.");
    }
    std::ofstream outstream(outpath);
    outstream << "t-x";
    for (auto &valuepair : first_step[q_it.key()]) {
      outstream << ",   " << valuepair["x"];
    }
    outstream << "\n";
    for (auto &timestep : object["data"]) {
      outstream << timestep["time"];
      for (auto &valuepair : timestep[q_it.key()]) {
        outstream << ",   " << valuepair["value"];
      }
      outstream << "\n";
    }
  }
}
