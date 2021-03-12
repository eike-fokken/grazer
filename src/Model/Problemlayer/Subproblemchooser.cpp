#include "Exception.hpp"
#include "Subproblemchooser.hpp"
#include "Networkproblem.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Net.hpp"
#include "Edge.hpp"
#include "Node.hpp"
#include "Full_factory.hpp"

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
  build_networkproblem(nlohmann::json &networkproblem_json) {

    auto net_ptr = Networkproblem::build_net<Componentfactory::Full_factory>(networkproblem_json);
    
    return std::make_unique<Networkproblem::Networkproblem>(std::move(net_ptr));
  }
} // namespace Model
