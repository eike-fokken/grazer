#include "Equationcomponent.hpp"
#include "Node.hpp"
#include <Networkproblem.hpp>
#include <memory>
#include <utility>
#include <Net.hpp>
#include <vector>

namespace Model::Networkproblem {

Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network)
    : network(std::move(_network)) {

  for(std::shared_ptr<Network::Node> node : network->get_nodes())
    {
      if(auto equationnode=std::dynamic_pointer_cast<Equationnode>(node).get())
        {
          equationnodes.push_back(equationnode);
        }
    }

    for (std::shared_ptr<Network::Edge> edge : network->get_edges()) {
      if (auto equationedge =
              std::dynamic_pointer_cast<Equationedge>(edge).get()) {
        equationedges.push_back(equationedge);
      }
    }
}




}
