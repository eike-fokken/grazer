#pragma once
#include "Controlcomponent.hpp"
#include "Costcomponent.hpp"
#include "Inequalitycomponent.hpp"
#include "Statecomponent.hpp"
#include <Eigen/Sparse>
#include <memory>
#include <vector>

namespace Aux {

  class Costgradienthandler;
}

namespace Network {
  class Net;
}

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

  class Equationcomponent;

  // This class defines a problem, that builds the model equations from a
  // network.
  class Networkproblem final :
      public Statecomponent,
      public Controlcomponent,
      public Inequalitycomponent,
      public Costcomponent {

  public:
    std::string get_type() const;

    ~Networkproblem() override; // override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);

    ////////////////////////////////////////////////////////////////////////////
    // Statecomponent methods
    ////////////////////////////////////////////////////////////////////////////

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) override;

    void add_results_to_json(nlohmann::json &new_output) override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) override;

    int set_state_indices(int next_free_index) override;

  private:
    ////////////////////////////////////////////////////////////////////////////
    // Controlcomponent methods
    ////////////////////////////////////////////////////////////////////////////
  public:
    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) override;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler &jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evalutate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void set_initial_controls_of_timestep(
        double time, Eigen::Ref<Eigen::VectorXd> controls,
        nlohmann::json const &control_json) override;

    void set_lower_bounds(Eigen::Ref<Eigen::VectorXd> lower_bounds) override;

    void set_upper_bounds(Eigen::Ref<Eigen::VectorXd> upper_bounds) override;

    int set_control_indices(int next_free_index) override;

    void setup() override;

    /////////////////////////////////////////////////////////
    // cost function methods:
    /////////////////////////////////////////////////////////
  public:
    void evaluate_cost(
        Eigen::Ref<Eigen::VectorXd> cost_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_cost_d_state(
        Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_cost_d_control(
        Aux::Costgradienthandler &cost_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    /////////////////////////////////////////////////////////
    // inequality methods:
    /////////////////////////////////////////////////////////

    void evaluate_inequality(
        Eigen::Ref<Eigen::VectorXd> inequality_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_inequality_d_state(
        Aux::Matrixhandler &inequality_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_inequality_d_control(
        Aux::Matrixhandler &inequality_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    int set_inequality_indices(int next_free_index) override;

    /////////////////////////////////////////////////////////
    // other methods:
    /////////////////////////////////////////////////////////
  public:
    Network::Net const &get_network() const;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;
    std::vector<Statecomponent *> statecomponents;
    std::vector<Controlcomponent *> controlcomponents;
    std::vector<Costcomponent *> costcomponents;
    std::vector<Inequalitycomponent *> inequalitycomponents;

    int reserve_control_indices(int next_free_index);
  };

} // namespace Model::Networkproblem
