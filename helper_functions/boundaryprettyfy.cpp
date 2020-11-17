#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::ordered_json;

// declarations to circumvent the warning: no prior declaration.
std::string getnodetype(json node);
json get_node_type_json(json boundarydata);
std::string getlintype(json node);

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

  if (argc != 7) {
    std::cout << "You must specify input files as: boundary_input.json "
                 "boundary_output.json topology_input.json "
                 "topology_output.json initial_input.json initial_output.json."
                 "on the command line!"
              << std::endl;
    return 1;
  }

  if (std::filesystem::exists(argv[2]) or std::filesystem::exists(argv[4]) or
      std::filesystem::exists(argv[6])) {
    std::cout << "one of the output files exists, overwrite?" << '\n';
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
  }

  json boundarydata;
  json topologydata;
  json initialdata;

  {
    std::ifstream boundary_filestream(argv[1]);
    boundary_filestream >> boundarydata;
  }
  {
    std::ifstream topology_filestream(argv[3]);
    topology_filestream >> topologydata;
  }

  {
    std::ifstream initial_filestream(argv[5]);
    initial_filestream >> initialdata;
  }

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

  for (auto &node : bdcond) {
    json newdata;
    for (auto &datum : node["data"]) {
      json newdatapoint;
      newdatapoint["time"] = datum[0];
      for (auto itr = ++datum.begin(); itr != datum.end(); ++itr) {
        newdatapoint["values"].push_back((*itr));
      }
      newdata.push_back(newdatapoint);
    }
    node["data"] = newdata;
  }

  newnetdata["boundarycondition"] = bdcond;

  json nodes = topologydata["nodes"];
  json PV;
  json PQ;
  json Vphi;
  for (auto &powernode : nodes["Powernode"]) {
    auto res =
        std::find_if(bdcond.begin(), bdcond.end(), [powernode](const json &x) {
          auto it = x.find("id");
          return it != x.end() and it.value() == powernode["id"];
        });
    std::string Gstring = powernode["G"];
    std::string Bstring = powernode["B"];
    double G = std::stod(Gstring);
    double B = std::stod(Bstring);
    powernode.erase("G");
    powernode.erase("B");
    powernode["G"] = G;
    powernode["B"] = B;
    if ((*res)["type"] == "PV") {
      PV.push_back(powernode);
    }
    if ((*res)["type"] == "PQ") {
      PQ.push_back(powernode);
    }
    if ((*res)["type"] == "Vphi") {
      Vphi.push_back(powernode);
    }
  }

  nodes["Vphi_nodes"] = Vphi;
  nodes["PV_nodes"] = PV;
  nodes["PQ_nodes"] = PQ;

  nodes.erase("Powernode");

  ////// Topology: remove strings, make doubles.
  topologydata["nodes"] = nodes;

  json tlines = topologydata["connections"]["transmissionLine"];

  for (auto &tline : tlines) {
    std::string Gstring = tline["G"];
    std::string Bstring = tline["B"];
    double G = std::stod(Gstring);
    double B = std::stod(Bstring);
    tline.erase("G");
    tline.erase("B");
    tline["G"] = G;
    tline["B"] = B;
  }

  topologydata["connections"]["transmissionLine"] = tlines;

  json initialconditions = initialdata["objectCondition"];

  for (auto &condition : initialconditions) {
    if (condition["data"].type() == json::value_t::array) {
      for (auto &datapoint : condition["data"]) {
        std::string xstring = datapoint["x"];
        double x = std::stod(xstring);
        datapoint["x"] = x;
        if (datapoint["value"].type() == json::value_t::array) {
          for (auto &valuepoint : datapoint["value"]) {
            std::string valuestring = valuepoint;
            double value = std::stod(valuestring);
            valuepoint = value;
          }
        } else if (datapoint["value"].type() == json::value_t::object) {
          std::string valuestring = datapoint["value"];
          double value = std::stod(valuestring);
          datapoint["value"] = value;
        }
      }
    } else if (condition["data"].type() == json::value_t::object) {
      std::string xstring = condition["data"]["x"];
      double x = std::stod(xstring);
      condition["data"]["x"] = x;
      if (condition["data"]["value"].type() == json::value_t::array) {
        for (auto &valuepoint : condition["data"]["value"]) {
          std::string valuestring = valuepoint;
          double value = std::stod(valuestring);
          valuepoint = value;
        }

      } else if (condition["data"]["value"].type() == json::value_t::object) {
        std::string valuestring = condition["data"]["value"];
        double value = std::stod(valuestring);
        condition["data"]["value"] = value;
      }
    }
  }

  std::ofstream obound(argv[2]);
  obound << std::setw(4) << newnetdata << std::endl;

  std::ofstream otop(argv[4]);
  otop << std::setw(4) << topologydata << std::endl;

  std::ofstream oinit(argv[6]);
  oinit << std::setw(4) << initialconditions << std::endl;

  return 0;
}

// for(auto & source: nodes["source"])
//   {
//     auto res = std::find_if(bdcond.begin(), bdcond.end(), [source](const
//     json& x) {
//                                                             auto it =
//                                                             x.find("id");
//                                                             return it !=
//                                                             x.end() and
//                                                             it.value() ==
//                                                             source["id"];
//                                                           });
//     source["type"] = (*res)["type"];
//   }
// for(auto & sink: nodes["sink"])
//   {
//     auto res = std::find_if(bdcond.begin(), bdcond.end(), [sink](const
//     json& x) {
//                                                             auto it =
//                                                             x.find("id");
//                                                             return it !=
//                                                             x.end() and
//                                                             it.value() ==
//                                                             sink["id"];
//                                                           });
//     sink["type"] = (*res)["type"];
//   }
