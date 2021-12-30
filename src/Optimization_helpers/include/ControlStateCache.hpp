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

  class ControlStateCache final {
  public:
    ControlStateCache(
        Model::Timeevolver &timeevolver, Model::Controlcomponent &problem);

    bool refresh_cache(
        Aux::InterpolatingVector_Base const &controls,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state);

    Aux::InterpolatingVector_Base const &get_cached_states();

    Aux::InterpolatingVector_Base const *check_and_supply_states(
        Aux::InterpolatingVector_Base const &controls,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state);

  private:
    Model::Timeevolver &evolver;
    Model::Controlcomponent &problem;

    std::pair<Cacheentry, Aux::InterpolatingVector> cache;
    Cacheentry failed;
  };

} // namespace Optimization
