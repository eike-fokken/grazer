#include "Subproblem.hpp"
#include <nlohmann/json.hpp>
#include <memory>


namespace Model {

  std::unique_ptr<Subproblem> build_subproblem(std::string subproblem_type, nlohmann::json & subproblem_json);

  std::unique_ptr<Subproblem>
  build_networkproblem(nlohmann::json & subproblem_json);
}
