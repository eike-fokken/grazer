#pragma once
#include <Subproblem.hpp>
#include <vector>
#include <memory>
namespace Model {


  class Generalnetwork : public Subproblem{

  public:


  private:
    std::vector<std::unique_ptr<Network::Edge> > edges;
    std::vector<std::unique_ptr<Network::Node> > nodes;
  };

} // namespace Model
