#include "Initialvalues.hpp"

namespace Optimization {

  Initialvalues::Initialvalues(
      Aux::InterpolatingVector _initial_controls,
      Aux::InterpolatingVector _lower_bounds,
      Aux::InterpolatingVector _upper_bounds,
      Aux::InterpolatingVector _constraint_lower_bounds,
      Aux::InterpolatingVector _constraint_upper_bounds) :
      initial_controls(std::move(_initial_controls)),
      lower_bounds(std::move(_lower_bounds)),
      upper_bounds(std::move(_upper_bounds)),
      constraint_lower_bounds(std::move(_constraint_lower_bounds)),
      constraint_upper_bounds(std::move(_constraint_upper_bounds)) {}

  bool Initialvalues::obsolete() const {
    return (
        supplied_initial_controls and supplied_lower_bounds
        and supplied_upper_bounds and supplied_constraint_lower_bounds
        and supplied_constraint_upper_bounds);
  }
} // namespace Optimization
