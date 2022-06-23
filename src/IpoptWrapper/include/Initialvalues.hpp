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
  };
} // namespace Optimization
