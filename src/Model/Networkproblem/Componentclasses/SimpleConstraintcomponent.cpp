#include "SimpleConstraintcomponent.hpp"
namespace Model {

  int SimpleConstraintcomponent::set_constraint_indices(int next_free_index) {
    start_constraint_index = next_free_index;
    int number_of_inequalities = needed_number_of_inequalities_per_time_point();
    after_constraint_index = next_free_index + number_of_inequalities;

    return after_constraint_index;
  }

} // namespace Model
