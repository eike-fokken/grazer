#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

void add_json_data(
    nlohmann::json const &input, nlohmann::json &output,
    std::vector<std::string> types);
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

      // auto id_is = [id](nlohmann::json const &a) -> bool {
      //   return a["id"].get<std::string>() == id;
      // };
      nlohmann::json this_id;
      this_id["id"] = id;
      auto it = std::lower_bound(
          output[type].begin(), output[type].end(), this_id, id_compare_less);
      if (it == output[type].end()) {

        nlohmann::json newoutput;
        newoutput["id"] = id;
        newoutput["data"] = component["data"];
        output[type].push_back(newoutput);
      } else {
        auto &olddata = (*it)["data"];
        auto &newdata = component["data"];
        auto oldit = olddata.begin();
        for (auto &step : newdata) {
          for (auto varit = step.begin(); varit != step.end(); ++varit) {
            if (varit.key() == "time") {
              continue;
            }
            (*oldit)[varit.key()] = (*oldit)[varit.key()].get<double>()
                                    + varit.value().get<double>();
          }
          // also make a time step through the already saved values:
          ++oldit;
        }
      }
    }
  }
}

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
    }
  }

  std::ofstream outstream(computed_output);
  outstream << output.dump(1);

  // {
  //   std::ofstream outputstream(outputfile);
  //   outputstream << input;
  // }
  // std::cout << "first part worked" << std::endl;
}
