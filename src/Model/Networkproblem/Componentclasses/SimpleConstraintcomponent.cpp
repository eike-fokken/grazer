#include "SimpleConstraintcomponent.hpp"
#include "InterpolatingVector.hpp"
namespace Model {

  void set_simple_constraint_values(
      Constraintcomponent const *component,
      Aux::InterpolatingVector &full_constraint_vector,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {

    auto &timepoints = full_constraint_vector.get_interpolation_points();

    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);

    auto number_of_indices_per_step = initialvalues.get_inner_length();

    Eigen::Index index = 0;
    for (auto timepoint : timepoints) {
      full_constraint_vector.mut_timestep(index).segment(
          component->get_constraint_startindex(), number_of_indices_per_step)
          = initialvalues(timepoint);
      ++index;
    }
  }

  Eigen::Index SimpleConstraintcomponent::set_constraint_indices(
      Eigen::Index next_free_index) {
    constraint_startindex = next_free_index;
    auto number_of_inequalities = needed_number_of_constraints_per_time_point();
    constraint_afterindex = next_free_index + number_of_inequalities;

    return constraint_afterindex;
  }

} // namespace Model
