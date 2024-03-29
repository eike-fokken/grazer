/** @file */

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stdexcept>
#include <utility>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

struct Quantile_triplet {
  double quantile;
  double low_value;
  double high_value;
};

static std::vector<Quantile_triplet> make_quantiles(json::array_t const &data);

static void add_power_json_data(
    json const &input, json &output, std::vector<std::string> types);
static void add_gas_json_data(
    json const &input, json &output, std::vector<std::string> types);

static void
compute_actual_quantiles_gas(json &output, std::vector<std::string> types);
static void
compute_actual_quantiles_power(json &output, std::vector<std::string> types);

//////** \brief Computes quantiles for a collection of grazer outputs.
///// *
///// * This only makes sense if grazer was called on stochastic inputs (like
///// * StochasticPQnode). For the output to be meaningful, it is also important
///// * that the same problem was solved with grazer for each of the outputs.
///// *
///// * All arguments are optional. If one or more arguments is given, the first
/// one
///// * is given as the common substring with which all output files start. If
///// * two arguments are given, the second argument is the filename of the json
///// * file, get_quantiles should create, to output its computations. The
/// default
///// * value of the substring is "output". The default value of the filename is
///// * "quantiles.json".
///// */

/** \brief This is currently broken, as the directory layout of the output files
 * changed!
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

  for (auto &pit : fs::directory_iterator(
           fs::current_path() / fs::path(output_filename_trunk))) {
    // ignore directories starting with . (hidden files in unix)
    if ((pit.path().filename().string()).find(".") == 0) {
      continue;
    }
    // if ((pit.path().filename().string()).find(output_filename_trunk) == 0)
    {
      std::cout << "Processing " << pit.path().filename() << std::endl;

      json input;
      {
        fs::path currpath = fs::absolute(pit.path());
        std::ifstream inputstream(currpath);
        inputstream >> input;
      }
      try {
        add_power_json_data(input, output, powertypes);
        add_gas_json_data(input, output, gastypes);
      } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
      }
    }
  }
  compute_actual_quantiles_power(output, powertypes);
  compute_actual_quantiles_gas(output, gastypes);
  std::ofstream outstream(computed_output);
  outstream << output.dump(1, '\t');
}

void add_gas_json_data(
    json const &input, json &output, std::vector<std::string> types) {

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
        auto id_compare_less = [](json const &a, json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };

        json this_id;
        this_id["id"] = id;

        auto it_output = std::lower_bound(
            output[compclass][type].begin(), output[compclass][type].end(),
            this_id, id_compare_less);

        if (it_output == output[compclass][type].end()) {
          json newoutput;
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

                (*saved_point_it)["value"].push_back(
                    value_pair["value"].get<double>());
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

void add_power_json_data(
    json const &input, json &output, std::vector<std::string> types) {

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
        auto id_compare_less = [](json const &a, json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };

        json this_id;
        this_id["id"] = id;

        auto it_output = std::lower_bound(
            output[compclass][type].begin(), output[compclass][type].end(),
            this_id, id_compare_less);

        if (it_output == output[compclass][type].end()) {
          json newoutput;
          newoutput["id"] = id;

          for (unsigned int i = 0; i != component["data"].size(); ++i) {
            auto &step = component["data"][i];
            json time_step_to_save;
            for (auto &[key, value] : step.items()) {
              if (key == "time") {
                time_step_to_save["time"] = value;
                continue;
              }
              time_step_to_save[key].push_back(value);
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

          for (unsigned int j = 0; j != data_to_add.size(); ++j) {
            for (auto &[key, value] : data_to_add[j].items()) {
              if (key == "time") {
                continue;
              }
              data_to_save[j][key].push_back(value.get<double>());
            }
            // also make a time step through the already saved values:
            ++data_to_save_it;
          }
        }
      }
    }
  }
}

void compute_actual_quantiles_gas(
    json &output, std::vector<std::string> types) {

  for (auto const &compclass : {"nodes", "connections"}) {
    if (not output.contains(compclass)) {
      continue;
    }
    for (auto &type : types) {
      if (not output[compclass].contains(type)) {
        continue;
      }

      for (auto &component : output[compclass][type]) {
        for (auto &datapoint : component["data"]) {
          json son;
          for (auto &[key, value] : datapoint.items()) {
            if (key == "time") {
              son[key] = value;
              continue;
            }
            auto mediankey = key + "_median";
            auto q_one_low = key + "_q_one_low";
            auto q_one_high = key + "_q_one_high";

            auto q_two_low = key + "_q_two_low";
            auto q_two_high = key + "_q_two_high";
            auto q_three_low = key + "_q_three_low";
            auto q_three_high = key + "_q_three_high";

            for (unsigned int j = 0; j != value.size(); ++j) {
              std::sort(value[j]["value"].begin(), value[j]["value"].end());
              auto vector = make_quantiles(value[j]["value"]);

              son[mediankey][j]["x"] = value[j]["x"];
              son[mediankey][j]["value"] = vector[0].high_value;

              son[q_one_low][j]["x"] = value[j]["x"];
              son[q_one_low][j]["value"] = vector[1].low_value;
              son[q_one_high][j]["x"] = value[j]["x"];
              son[q_one_high][j]["value"] = vector[1].high_value;

              son[q_two_low][j]["x"] = value[j]["x"];
              son[q_two_low][j]["value"] = vector[2].low_value;
              son[q_two_high][j]["x"] = value[j]["x"];
              son[q_two_high][j]["value"] = vector[2].high_value;

              son[q_three_low][j]["x"] = value[j]["x"];
              son[q_three_low][j]["value"] = vector[3].low_value;
              son[q_three_high][j]["x"] = value[j]["x"];
              son[q_three_high][j]["value"] = vector[3].high_value;
            }
          }
          datapoint = std::move(son);
        }
      }
    }
  }
}

void compute_actual_quantiles_power(
    json &output, std::vector<std::string> types) {

  for (auto const &compclass : {"nodes", "connections"}) {
    if (not output.contains(compclass)) {
      continue;
    }
    for (auto &type : types) {
      if (not output[compclass].contains(type)) {
        continue;
      }

      for (auto &component : output[compclass][type]) {
        for (auto &datapoint : component["data"]) {
          json son;
          for (auto &[key, value] : datapoint.items()) {
            if (key == "time") {
              son[key] = value;
              continue;
            }
            std::sort(value.begin(), value.end());
            auto vector = make_quantiles(value);

            auto mediankey = key + "_median";
            auto q_one_low = key + "_q_one_low";
            auto q_one_high = key + "_q_one_high";
            auto q_two_low = key + "_q_two_low";
            auto q_two_high = key + "_q_two_high";
            auto q_three_low = key + "_q_three_low";
            auto q_three_high = key + "_q_three_high";

            son[mediankey] = vector[0].high_value;
            son[q_one_low] = vector[1].low_value;
            son[q_one_high] = vector[1].high_value;
            son[q_two_low] = vector[2].low_value;
            son[q_two_high] = vector[2].high_value;
            son[q_three_low] = vector[3].low_value;
            son[q_three_high] = vector[3].high_value;
          }
          datapoint = std::move(son);
        }
      }
    }
  }
}

std::vector<Quantile_triplet> make_quantiles(json::array_t const &data) {

  std::vector<Quantile_triplet> values;
  // double median;

  size_t n = data.size();
  // if (n % 2 == 1) {
  //   median = data[(n + 1) / 2];
  // } else {
  //   median = 0.5 * (data[n / 2].get<double>() + data[n / 2 +
  //   1].get<double>());
  // }

  // values.push_back({0, median, median});

  std::vector<double> quantile_values = {0.0, 0.5, 0.75, 0.9};
  for (auto &qvalue : quantile_values) {
    auto offset = qvalue * 0.5 * static_cast<unsigned int>(n);
    unsigned int highindex = static_cast<unsigned int>(
        std::round(0.5 * static_cast<unsigned int>(n) + offset));
    unsigned int lowindex = static_cast<unsigned int>(
        std::round(0.5 * static_cast<unsigned int>(n) - offset));

    values.push_back({qvalue, data[lowindex], data[highindex]});
  }
  return values;
}
