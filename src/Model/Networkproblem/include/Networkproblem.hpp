#pragma once
#include "Equationcomponent.hpp"
#include <Eigen/Sparse>
#include <Net.hpp>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

  // This class implements a Subproblem, that builds the model equations from a
  // network.
  class Networkproblem final : public Subproblem {

  public:
    ~Networkproblem() override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);

    void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state) final;
    void evaluate_state_derivative(Aux::Matrixhandler *jacobian,
                                   double last_time, double new_time,
                                   Eigen::VectorXd const &last_state,
                                   Eigen::VectorXd const &new_state) final;

    void display() override;

    std::map<std::string, std::pair<int, int>>
    get_initializer_list() const final;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;

    int reserve_indices(int const next_free_index) final;
  };

} // namespace Model::Networkproblem
