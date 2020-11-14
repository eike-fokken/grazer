#include <Problem.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <model.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
int main() {
  using json = nlohmann::ordered_json;
  {
    json netdata;

    std::ifstream jsonfilestream("data/greek_boundary_test.json");

    jsonfilestream >> netdata;

    json newnetdata;

    newnetdata["boundarycondition"] = json::array();

    for (auto itr = netdata["linearBoundaryCondition"].begin();
         itr != netdata["linearBoundaryCondition"].end(); ++itr) {
      json node;
      node["id"] = (*itr)["@id"];
      double lin0 = std::stod((*itr)["lin"][0].get_ref<std::string &>());
      double lin1 = std::stod((*itr)["lin"][1].get_ref<std::string &>());
      node["lin"] = {lin0, lin1};
      json data = (*itr)["data"];
      node["data"]["times"] = json::array();
      node["data"]["values"] = json::array();
      for (auto date = data.begin(); date != data.end(); ++date) {
        double time = std::stod((*date)["time"].get_ref<std::string &>());
        double value = std::stod((*date)["value"].get_ref<std::string &>());
        node["data"]["times"].push_back(time);
        node["data"]["values"].push_back(value);
      }
      newnetdata["boundarycondition"].push_back(node);
    }
    std::cout << newnetdata["boundarycondition"][0];

    std::ofstream o("data/pretty.json");
    o << std::setw(4) << newnetdata << std::endl;
  }
  {
    json newjson;
    std::ifstream jsonfilestream("data/pretty.json");

    jsonfilestream >> newjson;

    std::vector<double> times;
    std::vector<double> values;

    times = newjson["boundarycondition"][0]["data"]["times"]
                .get<std::vector<double>>();

    for (auto it = times.begin(); it != times.end(); ++it) {
      std::cout << *it << '\n';
    }
  }
  json boundary;

  std::ifstream jsonfilestream("data/stationary.json");
  jsonfilestream >> boundary;

  std::ofstream o("data/stationary.json");
  o << std::setw(4) << boundary << std::endl;
}

// #include <grazer.hpp>

// namespace Grazer {

// int solver() {}

// } // namespace Grazer
