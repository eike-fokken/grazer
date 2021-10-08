#include "Controlcomponent.hpp"

namespace Model::Networkproblem {
  void Controlcomponent::prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) {}

  int Controlcomponent::get_start_control_index() const {
    return start_control_index;
  }
  int Controlcomponent::get_after_control_index() const {
    return after_control_index;
  }
} // namespace Model::Networkproblem
