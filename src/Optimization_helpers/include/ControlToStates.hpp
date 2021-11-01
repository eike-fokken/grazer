#pragma once
#include "InterpolatingVector.hpp"
#include "Timeevolver.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace Optimization {

  class ControlToStates {
  public:
    ControlToStates(nlohmann::json const &data);

    bool get_states(
        Aux::InterpolatingVector const &controls,
        Aux::InterpolatingVector &states);

  private:
    Model::Timeevolver evolver;

    std::pair<Aux::InterpolatingVector, Aux::InterpolatingVector> cache;
    Aux::InterpolatingVector last_failed;
  };

} // namespace Optimization
