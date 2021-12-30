#include "Implicit_Optimizer.hpp"
#include "OptimizableObject.hpp"

namespace Optimization {

  Eigen::Index Implicit_Optimizer::get_total_no_controls() const {
    return controls_per_step() * control_steps();
  }
  Eigen::Index Implicit_Optimizer::get_total_no_constraints() const {
    return constraints_per_step() * constraint_steps();
  }
  Eigen::Index Implicit_Optimizer::get_no_nnz_in_jacobian() const {
    return constraint_jacobian.nonZeros();
  }

  void Implicit_Optimizer::new_x() {
    states_up_to_date = false;
    derivatives_up_to_date = false;
  }

  bool Implicit_Optimizer::evaluate_constraints(
      Eigen::Ref<Eigen::VectorXd const> const &controls,
      Eigen::Ref<Eigen::VectorXd> constraints) {
    Aux::ConstMappedInterpolatingVector const fullcontrols(
        control_timepoints, controls_per_step(), controls.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    // get states:
    auto *state_pointer
        = cache.compute_states(fullcontrols, state_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    Aux::MappedInterpolatingVector fullconstraints(
        constraint_timepoints, constraints_per_step(), constraints.data(),
        static_cast<Eigen::Index>(get_total_no_constraints()));

    // fill the constraints vector for every (constraints-)timepoint
    for (Eigen::Index constraint_timeindex = 0;
         constraint_timeindex != fullconstraints.size();
         ++constraint_timeindex) {
      double time
          = fullconstraints.interpolation_point_at_index(constraint_timeindex);
      problem.evaluate_constraint(
          fullconstraints.mut_timestep(constraint_timeindex), time,
          states(time), fullcontrols(time));
    }
    return true;
  }

  ////////////////////////////////////////////////////////////
  // simple convenience methods:
  ////////////////////////////////////////////////////////////

  Eigen::Index Implicit_Optimizer::states_per_step() const {
    return problem.get_number_of_states();
  }
  Eigen::Index Implicit_Optimizer::controls_per_step() const {
    return problem.get_number_of_controls_per_timepoint();
  }
  Eigen::Index Implicit_Optimizer::constraints_per_step() const {
    return problem.get_number_of_constraints_per_timepoint();
  }
  Eigen::Index Implicit_Optimizer::state_steps() const {
    return state_timepoints.size();
  }
  Eigen::Index Implicit_Optimizer::control_steps() const {
    return control_timepoints.size();
  }
  Eigen::Index Implicit_Optimizer::constraint_steps() const {
    return constraint_timepoints.size();
  }

} // namespace Optimization
