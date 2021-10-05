#pragma once
#include "Eigen/Sparse"

namespace Aux {
  class Costgradienthandler;
  class Matrixhandler;
} // namespace Aux

namespace Model::Networkproblem {
  class Costcomponent {

  public:
    virtual ~Costcomponent(){};

    virtual void evaluate_cost(
        Eigen::Ref<Eigen::VectorXd> cost_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    virtual void d_evaluate_cost_d_new_state(
        Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    virtual void d_evaluate_cost_d_last_state(
        Aux::Matrixhandler &cost_last_state_jacobian_handler, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    virtual void d_evaluate_cost_d_new_control(
        Aux::Costgradienthandler &cost_new_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    virtual void d_evaluate_cost_d_last_control(
        Aux::Costgradienthandler &cost_last_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    double get_cost_weight();

  private:
    double cost_weight;
  };
} // namespace Model::Networkproblem
