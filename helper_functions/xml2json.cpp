#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

std::string getnodetype(json node) {

  std::vector<std::string> current_var = node["var"];

  std::vector<std::string> pressurevar = {"pressure"};
  std::vector<std::string> flowvar = {"flow"};
  std::vector<std::string> negflowvar = {"negflow"};
  std::vector<std::string> PQvar = {"P", "Q"};
  std::sort(PQvar.begin(), PQvar.end());
  std::vector<std::string> PVvar = {"P", "V"};
  std::sort(PVvar.begin(), PVvar.end());
  std::vector<std::string> Vphivar = {"V", "phi"};
  std::sort(Vphivar.begin(), Vphivar.end());

  if (current_var == pressurevar) {
    return "pressure";
  } else if (current_var == flowvar) {
    return "flow";
  } else if (current_var == negflowvar) {
    return "negflow";
  } else if (current_var == PQvar) {
    return "PQ";
  } else if (current_var == PVvar) {
    return "PV";
  } else if (current_var == Vphivar) {
    return "Vphi";
  } else {
    return "unkown";
  }
}

std::string getlintype(json node) {
  auto current_lin = node["lin"].get<std::vector<int>>();
  std::vector<int> pressurelin = {1, 0};
  std::vector<int> flowlin = {0, 1};
  std::vector<int> negflowlin = {0, -1};
  std::vector<int> Plin = {1, 0, 0, 0};
  std::vector<int> Qlin = {0, 1, 0, 0};
  std::vector<int> Vlin = {0, 0, 1, 0};
  std::vector<int> philin = {0, 0, 0, 1};

  if (current_lin == pressurelin) {
    return "pressure";
  } else if (current_lin == flowlin) {
    return "flow";
  } else if (current_lin == negflowlin) {
    return "negflow";
  } else if (current_lin == Plin) {
    return "P";
  } else if (current_lin == Qlin) {
    return "Q";
  } else if (current_lin == Vlin) {
    return "V";
  } else if (current_lin == philin) {
    return "phi";
  } else {
    return "unkown";
  }
}

json get_node_type_json(json boundarydata) {
  json newnetdata;

  std::map<std::string, std::vector<int>> lin;

  json conditions =
      boundarydata["boundaryConditions"]["linearBoundaryCondition"];

  json nodes;
  for (auto itr = conditions.begin(); itr != conditions.end(); ++itr) {

    auto res = std::find_if(nodes.begin(), nodes.end(), [itr](const json &x) {
      auto it = x.find("id");
      return it != x.end() and it.value() == (*itr)["id"];
    });
    std::string var = getlintype(*itr);
    if (res == nodes.end()) {
      json node;
      node["id"] = (*itr)["id"];
      node["var"].push_back(var);
      nodes.push_back(node);
    } else {
      (*res)["var"].push_back(var);
    }
  }

  newnetdata["boundarycondition"] = nodes;

  return newnetdata;
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

  json newnetdata = get_node_type_json(boundarydata);

  auto bdcond = newnetdata["boundarycondition"];
  json conditions =
      boundarydata["boundaryConditions"]["linearBoundaryCondition"];

  for (auto itr = conditions.begin(); itr != conditions.end(); ++itr) {
    auto res = std::find_if(bdcond.begin(), bdcond.end(), [itr](const json &x) {
      auto it = x.find("id");
      return it != x.end() and it.value() == (*itr)["id"];
    });

    auto nodetype = getnodetype(*res);
    if (nodetype == "pressure") {
      (*res)["type"] = nodetype;
      for (auto datapoint : (*itr)["data"]) {
        (*res)["data"].push_back({datapoint["time"], datapoint["value"]});
      }
    } else if (nodetype == "flow") {
      (*res)["type"] = nodetype;
      for (auto datapoint : (*itr)["data"]) {
        (*res)["data"].push_back({datapoint["time"], datapoint["value"]});
      }
    } else if (nodetype == "negflow") {
      (*res)["type"] = nodetype;
      for (auto datapoint : (*itr)["data"]) {
        (*res)["data"].push_back(
            {datapoint["time"], -datapoint["value"].get<double>()});
      }
    } else if (nodetype == "Vphi") {
      (*res)["type"] = nodetype;
      for (auto datapoint : (*itr)["data"]) {
        bool time_found = false;
        auto lintype = getlintype(*itr);
        for (auto present = (*res)["data"].begin();
             present != (*res)["data"].end(); ++present) {
          if ((*present)[0] == datapoint["time"]) {
            time_found = true;
            if (lintype == "V") {
              (*present)[1] = datapoint["value"];
            } else if (lintype == "phi") {
              (*present)[2] = datapoint["value"];
            }
          }
        }
        if (time_found == false) {

          if (lintype == "V") {
            (*res)["data"].push_back(
                {datapoint["time"], datapoint["value"].get<double>(), 0.0});
          } else if (lintype == "phi") {
            (*res)["data"].push_back(
                {datapoint["time"], 0.0, datapoint["value"].get<double>()});
          }
        }
      }
    }
  }

  newnetdata["boundarycondition"] = bdcond;
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
