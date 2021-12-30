#include "InterpolatingVector.hpp"

namespace Optimization {

  struct Initialvalues {
    Initialvalues(
        Aux::InterpolatingVector initial_controls,
        Aux::InterpolatingVector lower_bounds,
        Aux::InterpolatingVector upper_bounds,
        Aux::InterpolatingVector constraint_lower_bounds,
        Aux::InterpolatingVector constraint_upper_bounds);
    Aux::InterpolatingVector const initial_controls;
    Aux::InterpolatingVector const lower_bounds;
    Aux::InterpolatingVector const upper_bounds;
    Aux::InterpolatingVector const constraint_lower_bounds;
    Aux::InterpolatingVector const constraint_upper_bounds;

    bool obsolete() const;
    bool supplied_initial_controls = false;
    bool supplied_lower_bounds = false;
    bool supplied_upper_bounds = false;
    bool supplied_constraint_lower_bounds = false;
    bool supplied_constraint_upper_bounds = false;
  };
} // namespace Optimization
