#include <filesystem>
#include <nlohmann/json.hpp>
namespace Aux {
  class InterpolatingVector;
}
namespace Model {
  class Networkproblem;
  struct Timedata;
} // namespace Model

void setup_controls(
    Aux::InterpolatingVector &controls, Model::Networkproblem &problem,
    Model::Timedata &timedata, nlohmann::json const &all_json,
    std::filesystem::path const &problem_directory);
