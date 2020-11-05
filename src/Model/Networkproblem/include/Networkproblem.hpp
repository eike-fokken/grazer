#pragma once
#include "Equationcomponent.hpp"
#include <Subproblem.hpp>
#include <Eigen/Sparse>
#include <memory>
#include <vector>

//forward declarations:
namespace Network{
  class Net;
  } // namespace Network

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {


  // This class implements a Subproblem, that builds the model equations from a network.
  class Networkproblem: public Subproblem{

  public:
    virtual ~Networkproblem() override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);

    virtual void evaluate(double current_time, double next_time, const Eigen::VectorXd &current_state,
                          Eigen::VectorXd &new_state) override;
    virtual void evaluate_state_derivative(double current_time, double next_time, const Eigen::VectorXd &,
                                           Eigen::SparseMatrix<double> &) override;
    
  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationedge *> equationedges;
    std::vector<Equationnode *> equationnodes;

    virtual unsigned int
    reserve_indices(unsigned int const next_free_index) override;
  };

} // namespace Model
