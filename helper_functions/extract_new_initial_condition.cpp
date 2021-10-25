#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

json convert_resultdata_to_intialdata(json datapoint, std::string ctypename);
json powerinitial(json datapoint);
json gasinitial(json datapoint);

/** \brief Writes out the state at the given time passed in an results file into
 * a file that can be used as an initial value file.
 *
 * The first time step after the given time is used,
 * if the time doesn't appear in the output file.  If too great a time is
 * passed, the last time step is used.
 *
 * The function expects the following command line arguments:
 *
 * - the json file containing the results of a grazer computation
 * - the output file name of the output file to be written to
 * - the time at which the initial value should be extracted
 */
int main(int argc, char **argv) {
  std::filesystem::path inputfile;
  std::filesystem::path outputfile;
  double time;
  if (argc != 4) {
    std::cout
        << "You must provide an results json file, an output file and a time!"
        << std::endl;
    return 1;
  } else {
    inputfile = argv[1];
    outputfile = argv[2];
    time = std::stod(argv[3]);
  }

  if (not fs::exists(inputfile)) {
    std::cout << "The input file doesn't exist!" << std::endl;
    return 1;
  }
  if (fs::exists(outputfile)) {
    std::cout << "Overriting " << outputfile.string() << std::endl;
  }

  json results;
  {
    std::ifstream resultsstream(inputfile);
    resultsstream >> results;
  }

  json new_initial_values;
  for (auto const &[classname, componentclass] : results.items()) {
    for (auto const &[ctypename, componenttype] : componentclass.items()) {
      new_initial_values[classname][ctypename] = json::array();
      for (auto const &component : componenttype) {
        auto less_time = [](json const &a, json const &b) -> bool {
          return a["time"] < b["time"];
        };
        json time_compare_value;
        time_compare_value["time"] = time;
        auto it = std::lower_bound(
            component["data"].begin(), component["data"].end(),
            time_compare_value, less_time);
        if (it != component["data"].begin()) {
          --it;
        }
        json initialdata;
        try {
          initialdata = convert_resultdata_to_intialdata(*it, ctypename);
        } catch (std::exception &e) {
          std::cout << "Failed to extract initial data from results file at "
                       "object with id: "
                    << component["id"] << std::endl;
          std::cout << "The error message was: " << std::endl;
          std::cout << e.what() << std::endl;
          return 2;
        }
        json initialvalue;
        initialvalue["id"] = component["id"];
        initialvalue["data"] = initialdata;
        new_initial_values[classname][ctypename].push_back(initialvalue);
      };
    }
  }
  {
    std::ofstream outstream(outputfile);
    outstream << new_initial_values.dump(1, '\t');
  }
}

json convert_resultdata_to_intialdata(json datapoint, std::string ctypename) {

  std::vector<std::string> powertypes{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode", "ExternalPowerplant"};
  std::vector<std::string> gastypes{
      "Pipe", "Shortpipe", "Gaspowerconnection", "Compressorstation",
      "Controlvalve"};

  {

    auto name_it = std::find(powertypes.begin(), powertypes.end(), ctypename);
    if (name_it != powertypes.end()) {
      return powerinitial(datapoint);
    }
  }
  {
    auto name_it = std::find(gastypes.begin(), gastypes.end(), ctypename);
    if (name_it != gastypes.end()) {
      return gasinitial(datapoint);
    }
  }
  throw std::runtime_error(
      "Unknown componenttype, not a power node, not a gas edge!");
}

json powerinitial(json datapoint) {
  json init_data = json::array();
  json init_datapoint;
  init_datapoint["x"] = 0.0;
  init_datapoint["values"] = json::array();
  init_datapoint["values"].push_back(datapoint["P"]);
  init_datapoint["values"].push_back(datapoint["Q"]);
  init_datapoint["values"].push_back(datapoint["V"]);
  init_datapoint["values"].push_back(datapoint["phi"]);
  init_data.push_back(init_datapoint);
  return init_data;
}

json gasinitial(json datapoint) {
  json init_data = json::array();
  auto flow_it = datapoint["flow"].begin();
  auto pressure_it = datapoint["pressure"].begin();
  for (; flow_it != datapoint["flow"].end(); ++flow_it, ++pressure_it) {
    json init_datapoint;
    init_datapoint["x"] = (*flow_it)["x"];
    init_datapoint["values"] = json::array();
    init_datapoint["values"].push_back((*pressure_it)["value"]);
    init_datapoint["values"].push_back((*flow_it)["value"]);
    init_data.push_back(init_datapoint);
  }
  return init_data;
}
