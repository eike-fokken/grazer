/** @file */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

double new_mean(double old_mean, double new_value, double n);
double new_sigma(double old_sigma, double old_mean, double new_value, double n);
void add_power_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs);
void add_gas_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs);

/** \brief Computes mean and standard deviation of a collection of grazer
 * outputs.
 *
 * This only makes sense if grazer was called on stochastic inputs (like
 * StochasticPQnode). For the output to be meaningful, it is also important,
 * that the same problem was solved with grazer for each of the outputs.
 *
 * All arguments are optional. If one or more arguments is given, it is given as
 * the common substring with which all output directories start. If two
 * arguments are given, the second argument is the filename of the json file,
 * get_mean_sigma should create to output its computations.
 * The default value of the substring is "output".
 * The default value of the filename is "mean_and_sigma.json".
 */

int main(int argc, char **argv) {
  std::string output_dir_trunk;
  if (argc >= 2) {
    output_dir_trunk = argv[1];
  } else {
    output_dir_trunk = "output";
  }
  std::cout << output_dir_trunk << std::endl;

  std::string computed_output;
  if (argc >= 3) {
    computed_output = argv[2];
  } else {
    computed_output = "mean_and_sigma.json";
  }
  std::cout << computed_output << std::endl;

  json output;

  std::vector<std::string> powertypes{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode"};
  std::vector<std::string> gastypes{"Pipe"};

  int number_of_runs = 0;
  for (auto &pit : fs::directory_iterator(fs::current_path())) {
    // check only directories:
    if (not fs::is_directory(pit.path())) {
      continue;
    }
    // ignore directories starting with . (hidden files in unix)
    if ((pit.path().filename().string()).find(".") == 0) {
      continue;
    }
    if ((pit.path().filename().string()).find(output_dir_trunk) == 0) {
      std::cout << "Processing " << pit.path().filename() << std::endl;

      json input;
      {
        fs::path currpath
            = fs::absolute(pit.path()) / fs::path("output.json").string();
        std::ifstream inputstream(currpath);
        inputstream >> input;
      }
      add_power_json_data(input, output, powertypes, number_of_runs);
      add_gas_json_data(input, output, gastypes, number_of_runs);
      ++number_of_runs;
    }
  }
  std::ofstream outstream(computed_output);
  outstream << output.dump(1);
}

void add_power_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs) {

  for (auto &type : types) {
    if (not input.contains(type)) {
      continue;
    }
    for (auto &component : input[type]) {
      auto id = component["id"];
      auto id_compare_less
          = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
        return a["id"].get<std::string>() < b["id"].get<std::string>();
      };

      nlohmann::json this_id;
      this_id["id"] = id;
      auto it = std::lower_bound(
          output[type].begin(), output[type].end(), this_id, id_compare_less);
      if (it == output[type].end()) {

        nlohmann::json newoutput;
        newoutput["id"] = id;

        auto &data_to_save = newoutput["data"];
        auto &newdata = component["data"];
        for (auto &step : newdata) {
          json time_step_to_save;
          for (auto varit = step.begin(); varit != step.end(); ++varit) {
            if (varit.key() == "time") {
              time_step_to_save[varit.key()] = varit.value();
              continue;
            }
            auto meankey = varit.key() + "_mean";
            auto sigmakey = varit.key() + "_sigma";
            time_step_to_save[meankey] = varit.value();
            time_step_to_save[sigmakey] = 0;
          }
          data_to_save.push_back(time_step_to_save);
        }
        output[type].push_back(newoutput);

      } else {
        if ((*it)["id"] != id) {
          throw std::runtime_error(
              "input wasn't sorted. is this really an output file of Grazer? "
              "If yes, file a bug!");
        }
        auto &olddata = (*it)["data"];
        auto &newdata = component["data"];
        auto oldit = olddata.begin();
        for (auto &step : newdata) {
          for (auto varit = step.begin(); varit != step.end(); ++varit) {
            if (varit.key() == "time") {
              continue;
            }
            auto meankey = varit.key() + "_mean";
            auto sigmakey = varit.key() + "_sigma";

            double n = number_of_runs;
            double new_value = varit.value().get<double>();
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
void add_gas_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types, int number_of_runs) {
  for (auto &type : types) {
    if (not input.contains(type)) {
      continue;
    }
    for (auto &component : input[type]) {
      auto id = component["id"];
      auto id_compare_less
          = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
        return a["id"].get<std::string>() < b["id"].get<std::string>();
      };

      nlohmann::json this_id;
      this_id["id"] = id;
      auto it = std::lower_bound(
          output[type].begin(), output[type].end(), this_id, id_compare_less);

      if (it == output[type].end()) {
        nlohmann::json newoutput;
        newoutput["id"] = id;
        auto &data_to_save = newoutput["data"];
        auto &data_to_add = component["data"];
        for (auto &step : data_to_add) {
          json time_step_to_save;
          for (auto varit = step.begin(); varit != step.end(); ++varit) {
            if (varit.key() == "time") {
              time_step_to_save["time"] = varit.value();
              continue;
            }
            auto meankey = varit.key() + "_mean";
            time_step_to_save[meankey] = varit.value();
            auto sigmakey = varit.key() + "_sigma";
            for (auto &value_pair_to_add : varit.value()) {
              json value_pair_to_save;
              value_pair_to_save["x"] = value_pair_to_add["x"];
              value_pair_to_save["value"] = 0.0;
              time_step_to_save[sigmakey].push_back(value_pair_to_save);
            }
          }
          data_to_save.push_back(time_step_to_save);
        }
        output[type].push_back(newoutput);

      } else {
        if ((*it)["id"] != id) {
          throw std::runtime_error(
              "input wasn't sorted. Is this really an output file of Grazer? "
              "If yes, file a bug!");
        }
        auto &data_to_save = (*it)["data"];
        auto &data_to_add = component["data"];
        auto data_to_save_it = data_to_save.begin();
        for (auto &step : data_to_add) {
          for (auto varit = step.begin(); varit != step.end(); ++varit) {
            if (varit.key() == "time") {
              continue;
            }
            auto meankey = varit.key() + "_mean";
            auto sigmakey = varit.key() + "_sigma";
            for (auto &value_pair : varit.value()) {
              auto x = value_pair["x"];
              auto compare_for_x = [](json const &a, json const &b) -> bool {
                return a["x"].get<double>() < b["x"].get<double>();
              };
              auto current_mean_pair_it = std::lower_bound(
                  (*data_to_save_it)[meankey].begin(),
                  (*data_to_save_it)[meankey].end(), value_pair, compare_for_x);
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
              double old_mean = (*current_mean_pair_it)["value"].get<double>();
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
double new_mean(double old_mean, double new_value, double n) {
  return n / (n + 1.0) * old_mean + 1.0 / (n + 1.0) * new_value;
}
double
new_sigma(double old_sigma, double old_mean, double new_value, double n) {
  return sqrt(
      (n - 1.0) / n * old_sigma * old_sigma
      + 1.0 / (n + 1.0) * (new_value - old_mean) * (new_value - old_mean));
}
