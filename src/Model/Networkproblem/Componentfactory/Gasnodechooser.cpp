#include "Gasnodechooser.hpp"
#include "Flowboundarynode.hpp"
#include "Innode.hpp"
#include "Sink.hpp"
#include "Source.hpp"

namespace Model::Networkproblem::Componentfactory {

  std::unique_ptr<Network::Node> build_innode(nlohmann::json const &topology) {
    if (topology.contains("couplingtype") &&
        topology["couplingtype"] == "Bernoulli") {
      return std::make_unique<Gas::Innode<Gas::Bernoulligasnode>>(topology);
    } else {
      return std::make_unique<Gas::Innode<Gas::Gasnode>>(topology);
    }
  }

  std::unique_ptr<Network::Node> build_source(nlohmann::json const &topology) {
    if (topology.contains("couplingtype") &&
        topology["couplingtype"] == "Bernoulli") {
      return std::make_unique<Gas::Source<Gas::Bernoullicouplingnode>>(
          topology);
    } else {
      return std::make_unique<Gas::Source<Gas::Pressurecouplingnode>>(topology);
    }
  }

  std::unique_ptr<Network::Node> build_sink(nlohmann::json const &topology) {
    if (topology.contains("couplingtype") &&
        topology["couplingtype"] == "Bernoulli") {
      return std::make_unique<Gas::Sink<Gas::Bernoullicouplingnode>>(
          topology);
    } else {
      return std::make_unique<Gas::Sink<Gas::Pressurecouplingnode>>(topology);
    }
  }
}
