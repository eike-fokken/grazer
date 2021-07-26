/** @file */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

static double new_mean(double old_mean, double new_value, double n);
static double
new_sigma(double old_sigma, double old_mean, double new_value, double n);
static void add_power_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs);
static void add_gas_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs);

/** \brief Computes mean and standard deviation of a collection of grazer
 * outputs.
 *
 * This only makes sense if grazer was called on stochastic inputs (like
 * StochasticPQnode). For the output to be meaningful, it is also important
 * that the same problem was solved with grazer for each of the outputs.
 *
 * All arguments are optional. If one or more arguments is given, the first one
 * is given as the common substring with which all output files start. If
 * two arguments are given, the second argument is the filename of the json
 * file, get_mean_sigma should create, to output its computations. The default
 * value of the substring is "output". The default value of the filename is
 * "mean_and_sigma.json".
 */

int main(int argc, char **argv) {
  std::string output_filename_trunk;
  if (argc >= 2) {
    output_filename_trunk = argv[1];
  } else {
    output_filename_trunk = "output";
  }
  std::cout << output_filename_trunk << std::endl;

  std::string computed_output;
  if (argc >= 3) {
    computed_output = argv[2];
  } else {
    computed_output = "mean_and_sigma.json";
  }
  std::cout << computed_output << std::endl;

  json output;

  std::vector<std::string> powertypes{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode", "ExternalPowerplant"};
  std::vector<std::string> gastypes{
      "Pipe", "Shortpipe", "Gaspowerconnection", "Compressorstation",
      "Controlvalve"};

  int number_of_runs = 0;
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
      add_power_json_data(input, output, powertypes, number_of_runs);
      add_gas_json_data(input, output, gastypes, number_of_runs);
      ++number_of_runs;
    }
  }
  std::ofstream outstream(computed_output);
  outstream << output.dump(1, '\t');
}

void add_power_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs) {

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
        auto it = std::lower_bound(
            output[compclass][type].begin(), output[compclass][type].end(),
            this_id, id_compare_less);
        if (it == output[compclass][type].end()) {

          nlohmann::json newoutput;
          newoutput["id"] = id;

          auto &data_to_save = newoutput["data"];
          auto &newdata = component["data"];
          for (auto &step : newdata) {
            json time_step_to_save;
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                time_step_to_save[key] = value;
                continue;
              }
              auto meankey = key + "_mean";
              auto sigmakey = key + "_sigma";
              time_step_to_save[meankey] = value;
              time_step_to_save[sigmakey] = 0;
            }
            data_to_save.push_back(time_step_to_save);
          }
          output[compclass][type].push_back(newoutput);

        } else {
          if ((*it)["id"] != id) {
            std::cout << "Expected id:" << id << std::endl;
            std::cout << "Gotten id:" << (*it)["id"] << std::endl;
            throw std::runtime_error(
                "input wasn't sorted or some outputs stem from different "
                "topology files than others!");
          }
          auto &olddata = (*it)["data"];
          auto &newdata = component["data"];
          auto oldit = olddata.begin();
          for (auto &step : newdata) {
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                continue;
              }
              auto meankey = key + "_mean";
              auto sigmakey = key + "_sigma";

              double n = number_of_runs;
              double new_value = value.get<double>();
              double old_mean = (*oldit)[meankey].get<double>();
              double old_sigma = (*oldit)[sigmakey].get<double>();

              (*oldit)[meankey] = new_mean(old_mean, new_value, n);
              (*oldit)[sigmakey] = new_sigma(old_sigma, old_mean, new_value, n);
            }
            // also make a time step through the already saved values:
            ++oldit;
          }
        }
      }
    }
  }
}
void add_gas_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs) {
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
        auto it = std::lower_bound(
            output[compclass][type].begin(), output[compclass][type].end(),
            this_id, id_compare_less);

        if (it == output[compclass][type].end()) {
          nlohmann::json newoutput;
          newoutput["id"] = id;
          auto &data_to_save = newoutput["data"];
          auto &data_to_add = component["data"];
          for (auto &step : data_to_add) {
            json time_step_to_save;
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                time_step_to_save["time"] = value;
                continue;
              }
              auto meankey = key + "_mean";
              time_step_to_save[meankey] = value;
              auto sigmakey = key + "_sigma";
              for (auto &value_pair_to_add : value) {
                json value_pair_to_save;
                value_pair_to_save["x"] = value_pair_to_add["x"];
                value_pair_to_save["value"] = 0.0;
                time_step_to_save[sigmakey].push_back(value_pair_to_save);
              }
            }
            data_to_save.push_back(time_step_to_save);
          }
          output[compclass][type].push_back(newoutput);

        } else {
          if ((*it)["id"] != id) {
            throw std::runtime_error(
                "input wasn't sorted. Is this really an output file of "
                "Grazer? "
                "If yes, file a bug!");
          }
          auto &data_to_save = (*it)["data"];
          auto &data_to_add = component["data"];
          auto data_to_save_it = data_to_save.begin();
          for (auto &step : data_to_add) {
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                continue;
              }
              auto meankey = key + "_mean";
              auto sigmakey = key + "_sigma";
              for (auto &value_pair : value) {
                auto x = value_pair["x"];
                auto compare_for_x = [](json const &a, json const &b) -> bool {
                  return a["x"].get<double>() < b["x"].get<double>();
                };
                auto current_mean_pair_it = std::lower_bound(
                    (*data_to_save_it)[meankey].begin(),
                    (*data_to_save_it)[meankey].end(), value_pair,
                    compare_for_x);
                auto current_sigma_pair_it = std::lower_bound(
                    (*data_to_save_it)[sigmakey].begin(),
                    (*data_to_save_it)[sigmakey].end(), value_pair,
                    compare_for_x);
                if ((*current_mean_pair_it)["x"] != x) {
                  throw std::runtime_error(
                      "The data wasn't sorted by x in component with id"
                      + id.get<std::string>());
                }
                if ((*current_sigma_pair_it)["x"] != x) {
                  throw std::runtime_error(
                      "The data wasn't sorted by x in component with id"
                      + id.get<std::string>());
                }

                double n = number_of_runs;
                double new_value = value_pair["value"];
                double old_mean
                    = (*current_mean_pair_it)["value"].get<double>();
                double old_sigma
                    = (*current_sigma_pair_it)["value"].get<double>();
                (*current_mean_pair_it)["value"]
                    = new_mean(old_mean, new_value, n);
                (*current_sigma_pair_it)["value"]
                    = new_sigma(old_sigma, old_mean, new_value, n);
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
double new_mean(double old_mean, double new_value, double n) {
  return n / (n + 1.0) * old_mean + 1.0 / (n + 1.0) * new_value;
}
double
new_sigma(double old_sigma, double old_mean, double new_value, double n) {
  return sqrt(
      (n - 1.0) / n * old_sigma * old_sigma
      + 1.0 / (n + 1.0) * (new_value - old_mean) * (new_value - old_mean));
}
