/** @file */

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

static void add_power_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types);
static void add_gas_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types);

/** \brief Computes quantiles for a collection of grazer outputs.
 *
 */

int main(int argc, char **argv) {
  std::string output_filename_trunk;

  if (argc >= 2) {
    output_filename_trunk = argv[1];
  } else {
    output_filename_trunk = "output";
  }
  std::cout << output_filename_trunk << std::endl;

  fs::path computed_output;
  if (argc >= 3) {
    computed_output = argv[2];
  } else {
    computed_output = "quantiles.json";
  }
  std::cout << computed_output << std::endl;

  json output;

  std::vector<std::string> powertypes{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode", "ExternalPowerplant"};
  std::vector<std::string> gastypes{
      "Pipe", "Shortpipe", "Gaspowerconnection", "Compressorstation",
      "Controlvalve"};

  for (auto &pit :
       fs::directory_iterator(fs::current_path() / fs::path("output"))) {
    // ignore directories starting with . (hidden files in unix)
    if ((pit.path().filename().string()).find(".") == 0) {
      continue;
    }
    if ((pit.path().filename().string()).find(output_filename_trunk) == 0) {
      std::cout << "Processing " << pit.path().filename() << std::endl;

      json input;
      {
        fs::path currpath = fs::absolute(pit.path());
        std::ifstream inputstream(currpath);
        inputstream >> input;
      }
      // add_power_json_data(input, output, powertypes);
      try {
        add_gas_json_data(input, output, gastypes);
      } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
      }
    }
  }
  std::ofstream outstream(computed_output);
  outstream << output.dump(1, '\t');
}

void add_gas_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types) {

  for (auto const &compclass : {"nodes", "connections"}) {
    if (not input.contains(compclass)) {
      continue;
    }
    for (auto &type : types) {
      if (not input[compclass].contains(type)) {
        continue;
      }

      for (auto &component : input[compclass][type]) {
        auto id = component["id"];
        auto id_compare_less
            = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };

        nlohmann::json this_id;
        this_id["id"] = id;

        auto it_output = std::lower_bound(
            output[compclass][type].begin(), output[compclass][type].end(),
            this_id, id_compare_less);

        if (it_output == output[compclass][type].end()) {
          nlohmann::json newoutput;
          newoutput["id"] = id;

          for (unsigned int i = 0; i != component["data"].size(); ++i) {
            auto &step = component["data"][i];
            json time_step_to_save;
            for (auto &[key, quantity] : step.items()) {
              if (key == "time") {
                time_step_to_save["time"] = quantity;
                continue;
              }
              for (unsigned int j = 0; j != quantity.size(); ++j) {
                time_step_to_save[key][j]["x"] = quantity[j]["x"];
                time_step_to_save[key][j]["value"].push_back(
                    quantity[j]["value"].get<double>());
              }
            }
            newoutput["data"].push_back(time_step_to_save);
          }
          output[compclass][type].push_back(newoutput);

        } else {
          if ((*it_output)["id"] != id) {
            throw std::runtime_error(
                "input wasn't sorted. Is this really an output file of "
                "Grazer? "
                "If yes, file a bug!");
          }
          auto &data_to_save = (*it_output)["data"];

          auto const &data_to_add = component["data"];

          auto data_to_save_it = data_to_save.begin();

          for (auto &step : data_to_add) {
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                continue;
              }
              for (auto const &value_pair : value) {
                auto x = value_pair["x"];
                auto compare_for_x = [](json const &a, json const &b) -> bool {
                  return a["x"].get<double>() < b["x"].get<double>();
                };
                auto saved_point_it = std::lower_bound(
                    (*data_to_save_it)[key].begin(),
                    (*data_to_save_it)[key].end(), value_pair, compare_for_x);

                if ((*saved_point_it)["x"] != x) {
                  throw std::runtime_error(
                      "The data wasn't sorted by x in component with id"
                      + id.get<std::string>());
                }

                data_to_save["value"].push_back(data_to_add["value"]);
              }
            }
            // also make a time step through the already saved values:
            ++data_to_save_it;
          }
        }
      }
    }
  }
}
