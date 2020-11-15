#include <Eigen/Dense>
#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace Model {

  class Problem;

} // namespace Model

namespace Jsonreader {
  using json = nlohmann::ordered_json;

  Model::Problem setup_problem(std::filesystem::path topology,
                               std::filesystem::path initial,
                               std::filesystem::path boundary);

  std::map<std::string, std::map<double, Eigen::Vector2d>>
  get_power_boundaries(json boundaryjson);

} // namespace Jsonreader
