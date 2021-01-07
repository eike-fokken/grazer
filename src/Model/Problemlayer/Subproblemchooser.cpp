#include "Exception.hpp"
#include <Subproblemchooser.hpp>

namespace Model {

  std::unique_ptr<Model::Subproblem>
  Subproblemchooser::build_subproblem(std::string subproblem_type,
                   nlohmann::json subproblem_json) {
    if(subproblem_type == "Network_problem"){
      return build_networkproblem(subproblem_json);
    }
    else {
      gthrow({"Encountered unknown subproblem type ", subproblem_type, ". Aborting now."});
    }
  }

  std::unique_ptr<Model::Networkproblem::Networkproblem>
  Subproblemchooser::build_networkproblem(nlohmann::json subproblem_json){
    return std::unique_ptr<Model::Networkproblem::Networkproblem>(new Networkproblem::Networkproblem(subproblem_json));
  }
} // namespace Model
