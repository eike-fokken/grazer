#include "SimpleConstraintcomponent.hpp"
namespace Model {

  Eigen::Index SimpleConstraintcomponent::set_constraint_indices(
      Eigen::Index next_free_index) {
    start_constraint_index = next_free_index;
    auto number_of_inequalities = needed_number_of_constraints_per_time_point();
    after_constraint_index = next_free_index + number_of_inequalities;

    return after_constraint_index;
  }

} // namespace Model
