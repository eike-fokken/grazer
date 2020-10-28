#pragma once
#include <Subproblem.hpp>
#include <Network/Net.hpp>
#include <Network/Edge.hpp>
#include <Network/Node.hpp>


#include <memory>
#include <vector>
    namespace Model {

  // This class implements a Subproblem, that builds the model equations from a network.
  class Generalnetwork : public Subproblem{

  public:


  private:
    std::vector<std::unique_ptr<Network::Edge> > edges;
    std::vector<std::unique_ptr<Network::Node> > nodes;
  };

} // namespace Model
