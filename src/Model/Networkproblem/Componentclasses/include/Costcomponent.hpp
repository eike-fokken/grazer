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

    virtual double evaluate_cost(
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evaluate_cost_d_state(
        Aux::Matrixhandler &cost_new_state_jacobian_handler, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evaluate_cost_d_control(
        Aux::Matrixhandler &cost_control_jacobian_handler, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    double get_cost_weight() const;

  private:
    double cost_weight{1.0};
  };
} // namespace Model
