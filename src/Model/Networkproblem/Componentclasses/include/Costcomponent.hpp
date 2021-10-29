#pragma once
#include "Eigen/Sparse"

namespace Aux {
  class Matrixhandler;
} // namespace Aux

namespace Model {
  class Costcomponent {

  public:
    Costcomponent();
    Costcomponent(double cost_weight);

    virtual ~Costcomponent(){};

    /**
     * @brief evaluates the cost component and returns the function value
     *
     * @param    last_time            time point of the last time step.
     * @param    new_time             time point of the current time step.
     * @param    state                value of the state at current time step.
     * @param    control              value of the control at current time step.
     * @return   double               The value of the cost of the component at
     * the current time step.
     */
    virtual double evaluate_cost(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /**
     * @brief derivative of Costcomponent::evaluate_cost w.r.t \p state.
     *
     * evaluates the derivative of Costcomponent::evaluate_cost and hands the
     * result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param    cost_new_state_jacobian_handler  A helper object, that fills a
     * sparse matrix in an efficient way.
     * @param    last_time            time point of the last time step. Usually
     * important for PDEs.
     * @param    new_time             time point of the current time step.
     * @param    state                value of the state at last time step.
     * @param    control              value of the control at current time step.
     */
    virtual void d_evaluate_cost_d_state(
        Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /**
     * @brief derivative of Costcomponent::evaluate w.r.t \p control.
     *
     * evaluates the derivative of Costcomponent::evaluate_cost and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param    cost_control_jacobian_handler  A helper object, that fills a
     * sparse matrix in an efficient way.
     * @param    last_time            time point of the last time step. Usually
     * important for PDEs.
     * @param    new_time             time point of the current time step.
     * @param    state                value of the state at last time step.
     * @param    control              value of the control at current time step.
     */
    virtual void d_evaluate_cost_d_control(
        Aux::Matrixhandler &cost_control_jacobian_handler, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    double get_cost_weight() const;

  private:
    double cost_weight{1.0};
  };
} // namespace Model
