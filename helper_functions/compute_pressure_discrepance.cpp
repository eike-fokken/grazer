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

// static void add_power_json_data(
//     nlohmann::json const &reference, nlohmann::json const &input,
//     nlohmann::json &output, std::vector<std::string> types);
void add_gas_json_data(
    nlohmann::json const &reference, nlohmann::json const &input,
    nlohmann::json &output, std::vector<std::string> types);

void add_power_json_data(
    nlohmann::json const &reference, nlohmann::json const &input,
    nlohmann::json &output, std::vector<std::string> types);

/** \brief Computes maximal discrepance between a given bottom line output
 * of a grazer problem and a collection of grazer outputs for the same
 * topology.
 *
 */

int main(int argc, char **argv) {
  std::string output_filename_trunk;

  if (argc < 2) {
    std::cout << "You must at least provide the reference filename."
              << std::endl;
    return EXIT_FAILURE;
  }

  json reference;
  {
    fs::path path_to_reference(argv[1]);
    std::ifstream inputstream(path_to_reference);
    inputstream >> reference;
  }

  if (argc >= 3) {
    output_filename_trunk = argv[2];
  } else {
    output_filename_trunk = "output";
  }
  std::cout << output_filename_trunk << std::endl;

  fs::path computed_output;
  if (argc >= 4) {
    computed_output = argv[3];
  } else {
    computed_output = "reference_discrepance.json";
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
      // add_power_json_data(reference, input, output, powertypes);
      try {
        add_gas_json_data(reference, input, output, gastypes);
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
    nlohmann::json const &reference, nlohmann::json const &input,
    nlohmann::json &output, std::vector<std::string> types) {

  for (auto const &compclass : {"nodes", "connections"}) {
    if (not reference.contains(compclass)) {
      continue;
    }
    if (not input.contains(compclass)) {
      throw std::runtime_error(
          "Some input lacks a field, the reference solution has, aborting!");
    }

    for (auto &type : types) {
      if (not reference[compclass].contains(type)) {
        continue;
      }
      if (not input[compclass].contains(type)) {
        std::cout << "missing type: " << type << std::endl;
        throw std::runtime_error(
            "Some input lacks types, the reference solution has, aborting!");
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

        auto it_reference = std::lower_bound(
            reference[compclass][type].begin(),
            reference[compclass][type].end(), this_id, id_compare_less);

        if (it_reference == reference[compclass][type].end()) {
          throw std::runtime_error(
              "The reference solution lacks a field that is present in some "
              "input file, aborting!");
        }
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
                time_step_to_save[key][j]["value"] = std::abs(
                    quantity[j]["value"].get<double>()
                    - (*it_reference)["data"][i][key][j]["value"]
                          .get<double>());
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
          if ((*it_reference)["id"] != id) {
            throw std::runtime_error(
                "reference wasn't sorted. Is this really an output file of "
                "Grazer? "
                "If yes, file a bug!");
          }

          auto &data_to_save = (*it_output)["data"];

          auto const &data_reference = (*it_reference)["data"];
          auto const &data_to_add = component["data"];

          auto data_to_save_it = data_to_save.begin();
          auto data_reference_it = data_reference.cbegin();

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
                auto reference_point_it = std::lower_bound(
                    (*data_reference_it)[key].cbegin(),
                    (*data_reference_it)[key].cend(), value_pair,
                    compare_for_x);

                if ((*saved_point_it)["x"] != x) {
                  throw std::runtime_error(
                      "The data wasn't sorted by x in component with id"
                      + id.get<std::string>());
                }
                if ((*reference_point_it)["x"] != x) {
                  throw std::runtime_error(
                      "The data wasn't sorted by x in component with id"
                      + id.get<std::string>());
                }

                double new_discrepance = std::abs(
                    value_pair["value"].get<double>()
                    - (*reference_point_it)["value"].get<double>());

                if (new_discrepance
                    > (*saved_point_it)["value"].get<double>()) {
                  (*saved_point_it)["value"] = new_discrepance;
                }
              }
            }
            // also make a time step through the already saved values:
            ++data_to_save_it;
            ++data_reference_it;
          }
        }
      }
    }
  }
}

void add_power_json_data(
    nlohmann::json const &reference, nlohmann::json const &input,
    nlohmann::json &output, std::vector<std::string> types) {

  for (auto const &compclass : {"nodes", "connections"}) {
    if (not reference.contains(compclass)) {
      continue;
    }
    if (not input.contains(compclass)) {
      throw std::runtime_error(
          "Some input lacks a field, the reference solution has, aborting!");
    }

    for (auto &type : types) {
      if (not reference[compclass].contains(type)) {
        continue;
      }
      if (not input[compclass].contains(type)) {
        std::cout << "missing type: " << type << std::endl;
        throw std::runtime_error(
            "Some input lacks types, the reference solution has, aborting!");
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

        auto it_reference = std::lower_bound(
            reference[compclass][type].begin(),
            reference[compclass][type].end(), this_id, id_compare_less);

        if (it_reference == reference[compclass][type].end()) {
          throw std::runtime_error(
              "The reference solution lacks a field that is present in some "
              "input file, aborting!");
        }
        if (it_output == output[compclass][type].end()) {
          nlohmann::json newoutput;
          newoutput["id"] = id;

          for (unsigned int i = 0; i != component["data"].size(); ++i) {
            auto &step = component["data"][i];
            json time_step_to_save;
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                time_step_to_save["time"] = value;
                continue;
              }
              ////////////////////////////////////////////////

              time_step_to_save[key] = std::abs(
                  value.get<double>()
                  - (*it_reference)["data"][i][key].get<double>());
              //////////////////////////////////////////////////////
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
          if ((*it_reference)["id"] != id) {
            throw std::runtime_error(
                "reference wasn't sorted. Is this really an output file of "
                "Grazer? "
                "If yes, file a bug!");
          }

          auto &data_to_save = (*it_output)["data"];

          auto const &data_reference = (*it_reference)["data"];
          auto const &data_to_add = component["data"];

          auto data_to_save_it = data_to_save.begin();
          auto data_reference_it = data_reference.cbegin();

          for (auto &step : data_to_add) {
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                continue;
              }
              //////////////////////////////////////////////
              double new_discrepance = std::abs(
                  value.get<double>()
                  - (*data_reference_it)[key].get<double>());

              if (new_discrepance > (*data_to_save_it)[key].get<double>()) {
                (*data_to_save_it)[key] = new_discrepance;
              }
              //////////////////////////////////////////////
            }
            // also make a time step through the already saved values:
            ++data_to_save_it;
            ++data_reference_it;
          }
        }
      }
    }
  }
}
