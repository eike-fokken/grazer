#pragma once
#include <Eigen/Sparse>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Network {
  class Net;
}

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

  class Equationcomponent;

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

    void save_values(double time, Eigen::VectorXd &new_state) final;

    void display() const override;

    virtual void set_initial_values(Eigen::VectorXd &new_state,
                                    nlohmann::ordered_json initial_json) final;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;

    int reserve_indices(int const next_free_index) final;
  };

} // namespace Model::Networkproblem
