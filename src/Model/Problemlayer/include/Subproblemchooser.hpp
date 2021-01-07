#include <Subproblem.hpp>
#include <nlohmann/json.hpp>
#include <any>
#include <functional>
#include <memory>
#include <Networkproblem.hpp>

namespace Model::Subproblemchooser {

  std::unique_ptr<Model::Subproblem> build_subproblem(std::string subproblem_type, nlohmann::json subproblem_json);

  std::unique_ptr<Model::Networkproblem::Networkproblem>
  build_networkproblem(nlohmann::json subproblem_json);
}
