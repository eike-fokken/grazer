#pragma once
#include <Subproblem.hpp>
#include <Net.hpp>
#include <Edge.hpp>
#include <Node.hpp>


#include <memory>
#include <vector>
    namespace Model {

  // This class implements a Subproblem, that builds the model equations from a network.
  class Networkproblem : public Subproblem{

  public:
    


  private:
    Network::Net network;
    std::vector<std::unique_ptr<Network::Edge> > equationedges;
    std::vector<std::unique_ptr<Network::Node> > equationnodes;
  };

} // namespace Model
