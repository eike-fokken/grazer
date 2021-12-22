#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <vector>

namespace Optimization {
  struct Cacheentry {
    Aux::InterpolatingVector control;
    Eigen::VectorXd state_interpolation_points;
    Eigen::VectorXd initial_state;
  };
} // namespace Optimization
