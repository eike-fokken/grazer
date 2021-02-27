#include "Gasnodechooser.hpp"

namespace Model::Networkproblem::Componentfactory {

  std::unique_ptr<Network::Node> build_innode(nlohmann::json const &topology) {
    if (topology.contains("couplingtype") &&
        topology["couplingtype"] == "Bernoulli") {
      return std::make_unique<Gas::Innode<Gas::Bernoulligasnode>>(topology);
    } else {
      return std::make_unique<Gas::Innode<Gas::Gasnode>>(topology);
    }
  }
}
