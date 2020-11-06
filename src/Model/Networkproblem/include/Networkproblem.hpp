#pragma once
#include "Equationcomponent.hpp"
#include <Eigen/Sparse>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

// forward declarations:
namespace Network {
class Net;
} // namespace Network

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

// This class implements a Subproblem, that builds the model equations from a
// network.
class Networkproblem : public Subproblem {

public:
  virtual ~Networkproblem() override{};

  Networkproblem(std::unique_ptr<Network::Net> _network);

  virtual void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                        double new_time, const Eigen::VectorXd &last_state,
                        Eigen::VectorXd const &new_state) override;
  virtual void
  evaluate_state_derivative(Eigen::SparseMatrix<double> &jacobian,
                            double last_time, double new_time,
                            const Eigen::VectorXd &last_state,
                            Eigen::VectorXd const &new_state) override;

private:
  std::unique_ptr<Network::Net> network;
  std::vector<Equationedge *> equationedges;
  std::vector<Equationnode *> equationnodes;

  virtual unsigned int
  reserve_indices(unsigned int const next_free_index) override;
};

} // namespace Model::Networkproblem
