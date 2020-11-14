#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::ordered_json;

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

  if (argc != 3) {
    std::cout << "You must specify an input json file and an outputjson file "
                 "on the command line!"
              << std::endl;
    return 1;
  }

  if (std::filesystem::exists(argv[2])) {
    std::cout << "output file exists, overwrite?" << '\n';
  }
  std::string answer;
  std::cin >> answer;

  if (answer == "n" or answer == "no") {
    std::cout << "Ok, I won't overwrite and exit now." << std::endl;
    return 0;
  }

  if (answer != "y" and answer != "yes") {
    std::cout << "You must answer yes/no or y/n, aborting!" << std::endl;
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
    } else if (nodetype == "PV") {
      (*res)["type"] = nodetype;
      for (auto datapoint : (*itr)["data"]) {
        bool time_found = false;
        auto lintype = getlintype(*itr);
        for (auto present = (*res)["data"].begin();
             present != (*res)["data"].end(); ++present) {
          if ((*present)[0] == datapoint["time"]) {
            time_found = true;
            if (lintype == "P") {
              (*present)[1] = datapoint["value"];
            } else if (lintype == "V") {
              (*present)[2] = datapoint["value"];
            }
          }
        }
        if (time_found == false) {

          if (lintype == "P") {
            (*res)["data"].push_back(
                {datapoint["time"], datapoint["value"].get<double>(), 0.0});
          } else if (lintype == "V") {
            (*res)["data"].push_back(
                {datapoint["time"], 0.0, datapoint["value"].get<double>()});
          }
        }
      }
    } else if (nodetype == "PQ") {
      (*res)["type"] = nodetype;
      for (auto datapoint : (*itr)["data"]) {
        bool time_found = false;
        auto lintype = getlintype(*itr);
        for (auto present = (*res)["data"].begin();
             present != (*res)["data"].end(); ++present) {
          if ((*present)[0] == datapoint["time"]) {
            time_found = true;
            if (lintype == "P") {
              (*present)[1] = datapoint["value"];
            } else if (lintype == "Q") {
              (*present)[2] = datapoint["value"];
            }
          }
        }
        if (time_found == false) {

          if (lintype == "P") {
            (*res)["data"].push_back(
                {datapoint["time"], datapoint["value"].get<double>(), 0.0});
          } else if (lintype == "Q") {
            (*res)["data"].push_back(
                {datapoint["time"], 0.0, datapoint["value"].get<double>()});
          }
        }
      }
    }
  }

  for (auto &node : bdcond) {
    node.erase("var");
  }
  newnetdata["boundarycondition"] = bdcond;
  std::ofstream o(argv[2]);
  o << std::setw(4) << newnetdata << std::endl;

  return 0;
}
