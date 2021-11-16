#pragma once
#include "Cacheentry.hpp"
#include "InterpolatingVector.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace Model {
  class Timeevolver;
  class Controlcomponent;
} // namespace Model

namespace Optimization {

  class ControlStateCache {
  public:
    ControlStateCache(
        Model::Timeevolver &timeevolver, Model::Controlcomponent &problem);

    Aux::InterpolatingVector_Base const *compute_states(
        Aux::InterpolatingVector_Base const &controls,
        std::vector<double> const &state_interpolation_points,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state);

  private:
    Model::Timeevolver &evolver;
    Model::Controlcomponent &problem;

    std::pair<Cacheentry, Aux::InterpolatingVector> cache;
    Cacheentry failed;
  };

} // namespace Optimization
