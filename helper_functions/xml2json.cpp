#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::ordered_json;

int getlintype(json node) {
  auto current_lin = node["lin"].get<std::vector<int>>();
  std::vector<int> pressurelin = {1, 0};
  std::vector<int> flowlin = {0, 1};
  std::vector<int> Plin = {1, 0, 0, 0};
  std::vector<int> Qlin = {0, 1, 0, 0};
  std::vector<int> Vlin = {0, 0, 1, 0};
  std::vector<int> philin = {0, 0, 0, 1};

  if (current_lin == pressurelin) {
    return 0;
  } else if (current_lin == flowlin) {
    return 1;
  } else if (current_lin == Plin) {
    return 2;
  } else if (current_lin == Qlin) {
    return 3;
  } else if (current_lin == Vlin) {
    return 4;
  } else if (current_lin == philin) {
    return 5;
  } else {
    return 100;
  }
}

int main(int argc, char *argv[]) {

  if (argc == 1) {
    std::cout << "You must specify an xml file on the command line!"
              << std::endl;
    return 1;
  }

  json boundarydata;

  std::ifstream jsonfilestream(argv[1]);

  jsonfilestream >> boundarydata;

  json newnetdata;

  newnetdata["boundarycondition"] = json::array();

  std::map<std::string, std::vector<int>> lin;

  json nodes;
  for (auto itr = boundarydata["boundaryConditions"]["linearBoundaryCondition"]
                      .begin();
       itr !=
       boundarydata["boundaryConditions"]["linearBoundaryCondition"].end();
       ++itr) {
    int lintype = getlintype(*itr);
    json node;
    switch (lintype) {
    case 0:
      node["var"].push_back("pressure");
      nodes.push_back(node);
      break;
    case 1:
      node["var"].push_back("flow");
      nodes.push_back(node);
      break;
    case 2:
      node["var"].push_back("P");
      nodes.push_back(node);
      break;
    case 3:
      node["var"].push_back("Q");
      nodes.push_back(node);
      break;
    case 4:
      node["var"].push_back("V");
      nodes.push_back(node);
      break;
    case 5:
      node["var"].push_back("phi");
      nodes.push_back(node);
      break;
    default:
      node["var"].push_back("unknown");
      nodes.push_back(node);
      break;
    }
  }
  newnetdata["boundarycondition"].push_back(nodes);

  std::ofstream o("data/testo.json");
  o << std::setw(4) << newnetdata << std::endl;
}

// if (current_lin == pressurelin) {
//   json node;
//   node["type"]="gaspressure";
//   nodes.push_back(node);
// } else if (current_lin == flowlin) {
//   json node;
//   node["type"] = "gasflow";
//   nodes.push_back(node);
// } else if (current_lin.size() == 4) {
//   auto liniterator = lin.find((*itr)["id"].get<std::string>());
//   if (liniterator ==lin.end()) {
//     lin[(*itr)["id"].get<std::string>()] = current_lin;
//   }
//   else {
//     std::vector<int> Plin = {1, 0, 0, 0};
//     std::vector<int> Qlin = {0, 1, 0, 0};
//     std::vector<int> Vlin = {0, 0, 1, 0};
//     std::vector<int> philin = {0, 0, 0, 1};
//     if ((current_lin == Plin && (*liniterator).second == Vlin) &&
//         (current_lin == Vlin && (*liniterator).second == Plin)) {
//       json node;
//       node["type"]="PV";
//       nodes.push_back(node);
//     }
//   }
//}
//       json data = (*itr)["data"];
//       node["data"]["times"] = json::array();
//       node["data"]["values"] = json::array();
//       for (auto date = data.begin(); date != data.end(); ++date) {
//         double time = std::stod((*date)["time"].get_ref<std::string &>());
//         double value = std::stod((*date)["value"].get_ref<std::string &>());
//         node["data"]["times"].push_back(time);
//         node["data"]["values"].push_back(value);
//       }
//       newnetdata["boundarycondition"].push_back(node);
//   }
//   std::cout << newnetdata["boundarycondition"][0];

//   std::ofstream o("data/pretty.json");
//   o << std::setw(4) << newnetdata << std::endl;
// }
// {
//   json newjson;
//   std::ifstream jsonfilestream("data/pretty.json");

//   jsonfilestream >> newjson;

//   std::vector<double> times;
//   std::vector<double> values;

//   times = newjson["boundarycondition"][0]["data"]["times"]
//               .get<std::vector<double>>();

//   for (auto it = times.begin(); it != times.end(); ++it) {
//     std::cout << *it << '\n';
//   }
// }
// json boundary;

// std::ifstream jsonfilestream("data/stationary.json");
// jsonfilestream >> boundary;

// std::ofstream o("data/stationary.json");
// o << std::setw(4) << boundary << std::endl;
//} //end main

// #include <grazer.hpp>

// namespace Grazer {

// int solver() {}
