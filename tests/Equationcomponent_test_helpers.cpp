#include "Equationcomponent_test_helpers.hpp"

nlohmann::json make_initial_json(
    std::map<std::string, std::vector<nlohmann::json>> nodemap,
    std::map<std::string, std::vector<nlohmann::json>> edgemap) {
  nlohmann::json initial_json;
  for (auto &[type, nodes] : nodemap) {
    for (auto &node : nodes) { initial_json["nodes"][type].push_back(node); }
  }

  for (auto &[type, edges] : edgemap) {
    for (auto &edge : edges) {
      initial_json["connections"][type].push_back(edge);
    }
  }
  return initial_json;
}

nlohmann::json make_full_json(
    std::map<std::string, std::vector<nlohmann::json>> nodemap,
    std::map<std::string, std::vector<nlohmann::json>> edgemap) {
  nlohmann::json full_json;
  full_json["boundary_json"] = nlohmann::json::object_t();
  full_json["control_json"] = nlohmann::json::object_t();
  full_json["topology_json"] = nlohmann::json::object_t();

  for (auto &[type, nodes] : nodemap) {
    for (auto &node : nodes) {
      full_json["topology_json"]["nodes"][type].push_back(node);
    }
  }

  for (auto &[type, edges] : edgemap) {
    for (auto &edge : edges) {
      full_json["topology_json"]["connections"][type].push_back(edge);
    }
  }
  return full_json;
}

std::unique_ptr<Model::Networkproblem> EqcomponentTEST::make_Networkproblem(
    nlohmann::json &netproblem,
    Model::Componentfactory::Componentfactory const &factory) {
  std::unique_ptr<Model::Networkproblem> netprob;
  {
    std::stringstream buffer;
    Catch_cout catcher(buffer.rdbuf());
    nlohmann::json outputjson;
    auto net_ptr = Model::build_net(netproblem, factory);
    netprob = std::make_unique<Model::Networkproblem>(std::move(net_ptr));
    output = buffer.str();
  }

  return netprob;
}
