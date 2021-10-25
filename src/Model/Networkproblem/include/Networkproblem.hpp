#pragma once
#include "Constraintcomponent.hpp"
#include "Controlcomponent.hpp"
#include "Costcomponent.hpp"
#include "Statecomponent.hpp"
#include "Timedata.hpp"
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
namespace Model {

  class Equationcomponent;

  // This class defines a problem, that builds the model equations from a
  // network.
  class Networkproblem final :
      public Statecomponent,
      public Controlcomponent,
      public Constraintcomponent,
      public Costcomponent {

  public:
    char const constexpr *get_type() const;

    ~Networkproblem() override; // override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);

    /** \brief Initializes a Networkproblem to a valid state.
     *
     * init() calls all of #set_state_indices(), #set_control_indices() and
     * #set_constraint_indices() on the appropriate components in
     * #statecomponents, #controlcomponents and #constraintcomponents. Also, it
     * calls #setup() on all elements in equationcomponents and
     * controlcomponents.
     *
     * @param next_free_state_index The greatest unclaimed index auf the state
     * vector (before calling this function), defaulted to 0.
     * @param next_free_control_index The greatest unclaimed index auf the
     * control vector (before calling this function), defaulted to 0.
     * @param next_free_constraint_index The greatest unclaimed index auf the
     * constraint vector (before calling this function), defaulted to 0.
     */
    void init(
        Eigen::Index next_free_state_index = 0,
        Eigen::Index next_free_control_index = 0,
        Eigen::Index next_free_constraint_index = 0);

    ////////////////////////////////////////////////////////////////////////////
    // Statecomponent methods
    ////////////////////////////////////////////////////////////////////////////

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) override;

    void add_results_to_json(nlohmann::json &new_output) override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) override;

    Eigen::Index set_state_indices(Eigen::Index next_free_index) override;

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

    void set_initial_controls(
        Aux::InterpolatingVector &full_control_vector,
        nlohmann::json const &control_json) const override;

    void set_lower_bounds(
        Aux::InterpolatingVector &full_control_vector,
        nlohmann::json const &control_json) const override;

    void set_upper_bounds(
        Aux::InterpolatingVector &full_control_vector,
        nlohmann::json const &control_json) const override;

    Eigen::Index set_control_indices(Eigen::Index next_free_index) override;

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
    // constraint methods:
    /////////////////////////////////////////////////////////

    void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    Eigen::Index set_constraint_indices(Eigen::Index next_free_index) override;

    void set_constraint_lower_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_lower_bounds,
        nlohmann::json const &constraint_lower_bound_json) override;

    void set_constraint_upper_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_upper_bounds,
        nlohmann::json const &constraint_upper_bound_json) override;

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
    std::vector<Constraintcomponent *> constraintcomponents;
  };

} // namespace Model
