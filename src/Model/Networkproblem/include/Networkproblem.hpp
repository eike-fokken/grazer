#pragma once
#include "Subproblem.hpp"
#include <Eigen/Sparse>
#include <memory>
#include <vector>

namespace Network {
  class Net;
}

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

  class Equationcomponent;
  class Statecomponent;

  // This class implements a Subproblem, that builds the model equations from a
  // network.
  class Networkproblem final : public Subproblem {

  public:
    std::string get_type() const override;

    ~Networkproblem() override; // override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const & last_state,
        Eigen::Ref<Eigen::VectorXd const> const & new_state,
        Eigen::Ref<Eigen::VectorXd const> const & last_control,
        Eigen::Ref<Eigen::VectorXd const> const & new_control) const final;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const & last_state,
        Eigen::Ref<Eigen::VectorXd const> const & new_state,
        Eigen::Ref<Eigen::VectorXd const> const & last_control,
        Eigen::Ref<Eigen::VectorXd const> const & new_control) override;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const & last_state,
        Eigen::Ref<Eigen::VectorXd const> const & new_state,
        Eigen::Ref<Eigen::VectorXd const> const & last_control,
        Eigen::Ref<Eigen::VectorXd const> const & new_control) const override;

    void json_save(double time, Eigen::Ref<Eigen::VectorXd const> const & state) final;

    void add_results_to_json(nlohmann::json &new_output) final;

    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json initial_json) final;

    Network::Net const &get_network() const;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;
    std::vector<Statecomponent *> statecomponents;

    int reserve_state_indices(int const next_free_index) final;
  };

} // namespace Model::Networkproblem
