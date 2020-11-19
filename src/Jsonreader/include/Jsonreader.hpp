#pragma once
#include "Net.hpp"
#include <Eigen/Dense>
#include <filesystem>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Model {

  class Problem;

} // namespace Model

namespace Network {
  class Node;
  class Edge;
} // namespace Network

/// This namespace contains utility functions to read data from json inputs into
/// the program.
namespace Jsonreader {
  using json = nlohmann::ordered_json;

  /// This function returns a full-fledged problem for solving with grazer.
  std::unique_ptr<Model::Problem> setup_problem(std::filesystem::path topology,
                                                std::filesystem::path boundary);

  /// This function reads boundary data for power nodes into a map for
  /// integration into the actual construction of Powernode s.
  /// It will be changed into accepting all double-valued boundary conditions
  /// should the need arise.
  std::map<std::string, std::map<double, Eigen::Vector2d>>
  get_two_value_boundaries(json const &boundaryjson);

  /// This function constructs a network from a vector of nodes, edges and
  /// boundary conditions. Up to now only double-valued boundary conditions (for
  /// power) are implemented.
  std::unique_ptr<Network::Net>
  construct_network(json &topologyjson, json &boundary_json,
                    std::vector<std::unique_ptr<Network::Node>> &nodes,
                    std::vector<std::unique_ptr<Network::Edge>> &edges);

  void set_initial_values(Eigen::VectorXd &initial_state,
                          std::filesystem::path initial,
                          std::unique_ptr<Model::Problem> &problem);

} // namespace Jsonreader
