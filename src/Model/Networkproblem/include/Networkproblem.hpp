#pragma once
#include "Subproblem.hpp"
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
  class Statecomponent;
  class Controlcomponent;
  class Costcomponent;
  class Inequalitycomponent;

  // This class implements a Subproblem, that builds the model equations from a
  // network.
  class Networkproblem final : public Subproblem {

  public:
    std::string get_type() const override;

    ~Networkproblem() override; // override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

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

    /** \brief derivative of evaluate w.r.t. \p control.
     *
     * evaluates the derivative of evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param last_control value of the control at last time step.
     * @param control value of the control at current time step.
     */
    void d_evalutate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /////////////////////////////////////////////////////////
    // cost function methods:
    /////////////////////////////////////////////////////////
    void evaluate_cost(
        Eigen::Ref<Eigen::VectorXd> cost_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const;

    void d_evaluate_cost_d_state(
        Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const;

    void d_evaluate_cost_d_control(
        Aux::Costgradienthandler &cost_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const;

    /////////////////////////////////////////////////////////
    // inequality methods:
    /////////////////////////////////////////////////////////

    void evaluate_inequality(
        Eigen::Ref<Eigen::VectorXd> inequality_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const;

    void d_evaluate_inequality_d_state(
        Aux::Matrixhandler &inequality_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const;

    void d_evaluate_inequality_d_control(
        Aux::Matrixhandler &inequality_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const;

    /////////////////////////////////////////////////////////
    // other methods:
    /////////////////////////////////////////////////////////

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;

    void add_results_to_json(nlohmann::json &new_output) final;

    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json initial_json) override;

    void set_initial_controls_of_timestep(
        double time, Eigen::Ref<Eigen::VectorXd> controls,
        nlohmann::json const &control_json) override;

    Network::Net const &get_network() const;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;
    std::vector<Statecomponent *> statecomponents;
    std::vector<Controlcomponent *> controlcomponents;
    std::vector<Costcomponent *> costcomponents;
    std::vector<Inequalitycomponent *> inequalitycomponents;

    int reserve_state_indices(int const next_free_index) final;
    int reserve_control_indices(int const next_free_index) final;
  };

} // namespace Model::Networkproblem
