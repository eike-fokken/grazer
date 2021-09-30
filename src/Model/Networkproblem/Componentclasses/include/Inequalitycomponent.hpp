#include "Eigen/Sparse"

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {
  class Inequalitycomponent {

  public:
    virtual ~Inequalitycomponent(){};

    virtual void evaluate_inequality(
        Eigen::Ref<Eigen::VectorXd> inequality_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_new_state(
        Aux::Matrixhandler *inequality_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_old_state(
        Aux::Matrixhandler *inequality_old_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_new_control(
        Aux::Matrixhandler *inequality_new_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_old_control(
        Aux::Matrixhandler *inequality_old_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;
  };
} // namespace Model::Networkproblem
