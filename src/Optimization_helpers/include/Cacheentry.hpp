#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <vector>

namespace Optimization {
  struct Cacheentry {
    Aux::InterpolatingVector control;
    std::vector<double> state_interpolation_points;
    Eigen::VectorXd initial_state;
  };
} // namespace Optimization
