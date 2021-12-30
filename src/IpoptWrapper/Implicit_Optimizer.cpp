#include "Implicit_Optimizer.hpp"
#include "ConstraintJacobian.hpp"
#include "Initialvalues.hpp"
#include "OptimizableObject.hpp"
#include <memory>

namespace Optimization {

  Implicit_Optimizer::Implicit_Optimizer(
      Model::OptimizableObject &_problem, Model::Timeevolver &_evolver,
      Eigen::VectorXd _state_timepoints, Eigen::VectorXd _control_timepoints,
      Eigen::VectorXd _constraint_timepoints, Eigen::VectorXd _initial_state,
      Aux::InterpolatingVector _initial_controls,
      Aux::InterpolatingVector _lower_bounds,
      Aux::InterpolatingVector _upper_bounds,
      Aux::InterpolatingVector _constraint_lower_bounds,
      Aux::InterpolatingVector _constraint_upper_bounds) :
      problem(_problem),
      init(std::make_unique<Initialvalues>(
          _initial_controls, _lower_bounds, _upper_bounds,
          _constraint_lower_bounds, _constraint_upper_bounds)),
      cache(_evolver, _problem),
      state_timepoints(_state_timepoints),
      control_timepoints(_control_timepoints),
      constraint_timepoints(_constraint_timepoints),
      initial_state(_initial_state),
      objective_gradient(control_timepoints, controls_per_step()),
      constraint_jacobian(
          constraints_per_step(), controls_per_step(), constraint_timepoints,
          control_timepoints) {}

  bool Implicit_Optimizer::supply_constraint_jacobian_indices(
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const {
    constraint_jacobian.supply_indices(Rowindices, Colindices);
    return true;
  }

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

  bool Implicit_Optimizer::evaluate_objective(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      double &objective) {
    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    auto *state_pointer
        = cache.compute_states(controls, state_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    objective = 0;
    // timeindex starts at 1, because at 0 there are initial conditions which
    // can not be altered!
    for (Eigen::Index timeindex = 1; timeindex != states.size(); ++timeindex) {
      objective += problem.evaluate_cost(
          state_timepoints[timeindex], states(state_timepoints[timeindex]),
          controls(state_timepoints[timeindex]));
    }
    return true;
  }

  bool Implicit_Optimizer::evaluate_constraints(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      Eigen::Ref<Eigen::VectorXd> ipoptconstraints) {
    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    // get states:
    auto *state_pointer
        = cache.compute_states(controls, state_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;
    Aux::MappedInterpolatingVector constraints(
        constraint_timepoints, constraints_per_step(), ipoptconstraints.data(),
        static_cast<Eigen::Index>(get_total_no_constraints()));

    // fill the constraints vector for every (constraints-)timepoint
    for (Eigen::Index constraint_timeindex = 0;
         constraint_timeindex != constraints.size(); ++constraint_timeindex) {
      double time
          = constraints.interpolation_point_at_index(constraint_timeindex);
      problem.evaluate_constraint(
          constraints.mut_timestep(constraint_timeindex), time, states(time),
          controls(time));
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
