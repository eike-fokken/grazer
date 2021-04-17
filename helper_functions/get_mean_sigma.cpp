#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

void add_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types);
// void compute_mean(
//     nlohmann::json &output, int number_of_runs, std::vector<std::string>
//     types);

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

  std::vector<std::string> types{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode"};

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
      add_json_data(input, output, types);
      ++number_of_runs;
    }
  }
  std::ofstream outstream(computed_output);
  outstream << output.dump(1);
}

void add_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types) {

  for (auto &type : types) {
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
            auto meanname = varit.key() + "_mean";
            auto sigmaname = varit.key() + "_sigma";
            time_step_to_save[meanname] = varit.value();
            time_step_to_save[sigmaname] = 0;
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
            (*oldit)[varit.key() + "_mean"]
                = (*oldit)[varit.key() + "_mean"].get<double>()
                  + varit.value().get<double>();
          }
          // also make a time step through the already saved values:
          ++oldit;
        }
      }
    }
  }
}

// void compute_mean(
//     nlohmann::json &output, int number_of_runs,
//     std::vector<std::string> types) {
//   for (auto &type : types) {
//     for (auto &component : output[type]) {
//       for (auto &step : component["data"]) {
//         for (auto varit = step.begin(); varit != step.end(); ++varit) {
//           if (varit.key() == "time") {
//             continue;
//           }
//           varit.value() = varit.value().get<double>() / number_of_runs;
//         }
//       }
//     }
//   }
// }
