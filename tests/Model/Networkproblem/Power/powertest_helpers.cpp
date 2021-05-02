#include "powertest_helpers.hpp"
#include "Equationcomponent_test_helpers.hpp"

nlohmann::json powernode_json(
    std::string id, double G, double B, Eigen::Vector2d bd0,
    Eigen::Vector2d bd1) {
  nlohmann::json powernode_json;
  powernode_json["id"] = id;
  powernode_json["G"] = G;
  powernode_json["B"] = B;
  auto b0 = make_value_json(id, "time", bd0, bd1);
  powernode_json["boundary_values"] = b0;
  return powernode_json;
}

nlohmann::json transmissionline_json(
    std::string id, double G, double B, nlohmann::json const &startnode,
    nlohmann::json const &endnode) {
  nlohmann::json transmissionline_json;
  transmissionline_json["id"] = id;
  transmissionline_json["G"] = G;
  transmissionline_json["B"] = B;
  transmissionline_json["from"] = startnode["id"];
  transmissionline_json["to"] = endnode["id"];

  return transmissionline_json;
}
