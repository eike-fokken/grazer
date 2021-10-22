#include "Constraintcomponent.hpp"
#include "Exception.hpp"

namespace Model {

  Eigen::Index
  Constraintcomponent::get_number_of_inequalities_per_timepoint() const {
    return get_after_constraint_index() - get_start_constraint_index();
  }

  /// \brief getter for #start_constraint_index
  Eigen::Index Constraintcomponent::get_start_constraint_index() const {
    if (start_constraint_index < 0) {
      gthrow(
          {"start_constraint_index < 0. Probably get_number_of_constraints() "
           "was called "
           "before calling set_constraint_indices().\n This is forbidden."});
    }

    return start_constraint_index;
  }

  /// \brief getter for #after_constraint_index
  Eigen::Index Constraintcomponent::get_after_constraint_index() const {
    if (after_constraint_index < 0) {
      gthrow(
          {"after_constraint_index < 0. Probably get_number_of_constraints() "
           "was called "
           "before calling set_constraint_indices().\n This is forbidden."});
    }

    return after_constraint_index;
  }
} // namespace Model
