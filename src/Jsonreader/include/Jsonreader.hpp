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
                                                std::filesystem::path initial,
                                                std::filesystem::path boundary);

  /// This function reads boundary data for power nodes into a map for
  /// integration into the actual construction of Powernode s.
  std::map<std::string, std::map<double, Eigen::Vector2d>>
  get_power_boundaries(json boundaryjson);

  Network::Net
  construct_network(json topologyjson,
                    std::map<std::string, std::map<double, Eigen::Vector2d>>
                        power_boundary_map);

} // namespace Jsonreader
