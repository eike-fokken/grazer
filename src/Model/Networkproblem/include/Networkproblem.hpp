#pragma once
#include <Subproblem.hpp>
#include <Eigen/Sparse>
#include <memory>
#include <vector>

//forward declaration:
namespace Network{
  class Net;
  class Edge;
  class Node;
}


// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

  // This class implements a Subproblem, that builds the model equations from a network.
  class Networkproblem : public Subproblem{

  public:
    virtual void evaluate(const Eigen::VectorXd &current_state,
                          Eigen::VectorXd &new_state);
    virtual void evaluate_state_derivative(const Eigen::VectorXd &,
                                           Eigen::SparseMatrix<double> &);
    virtual unsigned int get_number_of_states();

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<std::shared_ptr<Network::Edge> > equationedges;
    std::vector<std::shared_ptr<Network::Node> > equationnodes;
  };

} // namespace Model
