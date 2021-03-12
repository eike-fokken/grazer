#include "Exception.hpp"
#include "Subproblemchooser.hpp"
#include "Networkproblem.hpp"

namespace Model {

  std::unique_ptr<Subproblem>
  build_subproblem(std::string subproblem_type,
                   nlohmann::json &subproblem_json) {
    if(subproblem_type == "Network_problem"){
      return build_networkproblem(subproblem_json);
    }
    else {
      gthrow({"Encountered unknown subproblem type ", subproblem_type, ". Aborting now."});
    }
  }

  std::unique_ptr<Subproblem>
  build_networkproblem(nlohmann::json &subproblem_json) {
    //build the net here and just pass it to networkproblem!
    //
    return std::make_unique<Networkproblem::Networkproblem_base>(subproblem_json);
  }
} // namespace Model
