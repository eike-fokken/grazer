#include <Jsonreader.hpp>
#include <Problem.hpp>
#include <fstream>
#include <iostream>

namespace Jsonreader {

  using json = nlohmann::ordered_json;

  Model::Problem setup_problem(std::filesystem::path topology,
                               std::filesystem::path initial,
                               std::filesystem::path boundary) {

    json topologyjson;
    {
      std::ifstream jsonfilestream(topology);
      jsonfilestream >> topologyjson;
    }
    json initialjson;
    {
      std::ifstream jsonfilestream(initial);
      jsonfilestream >> initialjson;
    }
    json boundaryjson;
    {
      std::ifstream jsonfilestream(boundary);
      jsonfilestream >> boundaryjson;
    }

    std::map<std::string, std::map<double, Eigen::Vector2d>>
        power_boundary_map = get_power_boundaries(boundaryjson);
  }

  std::map<std::string, std::map<double, Eigen::Vector2d>>
  get_power_boundaries(json boundaryjson) {
    std::map<std::string, std::map<double, Eigen::Vector2d>>
        powerboundaryvalues;
    for (auto &node : boundaryjson["boundarycondition"]) {
      if (node["type"] == "PQ" or node["type"] == "PV" or
          node["type"] == "Vphi") {
        std::map<double, Eigen::Vector2d> node_boundary;
        for (auto &datapoint : node["data"]) {
          Eigen::Vector2d value;
          value[0] = datapoint[1];
          value[1] = datapoint[2];
          node_boundary.insert({datapoint[0], value});
        }
        powerboundaryvalues.insert(
            {node["id"].get<std::string>(), node_boundary});
      } else {
        std::cout << "not implemented yet: boundary values for nodes of type "
                  << node["type"] << " node: " << node["id"] << '\n';
      }
    }
    return powerboundaryvalues;
  }
} // namespace Jsonreader
