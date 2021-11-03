#pragma once
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

    bool get_states(
        Aux::InterpolatingVector const &controls,
        Aux::InterpolatingVector &states,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state);

  private:
    Model::Timeevolver &evolver;
    Model::Controlcomponent &problem;

    std::pair<Aux::InterpolatingVector, Aux::InterpolatingVector> cache;
    Aux::InterpolatingVector last_failed;
  };

} // namespace Optimization
