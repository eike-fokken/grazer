#include "Controlcomponent.hpp"
#include "Exception.hpp"

namespace Model::Networkproblem {
  void Controlcomponent::prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) {}

  int Controlcomponent::get_number_of_controls() const {
    if (start_control_index < 0) {
      gthrow(
          {"start_control_index < 0. Probably get_number_of_controls() was "
           "called "
           "before calling set_control_indices().\n This is forbidden."});
    }
    if (after_control_index < 0) {
      gthrow(
          {"after_control_index < 0. Probably get_number_of_controls() was "
           "called "
           "before calling set_control_indices().\n This is forbidden."});
    }
    return after_control_index - start_control_index;
  }

  int Controlcomponent::get_start_control_index() const {
    return start_control_index;
  }
  int Controlcomponent::get_after_control_index() const {
    return after_control_index;
  }
} // namespace Model::Networkproblem
