#pragma once
#include "InterpolatingVector.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace Model {
  class Timeevolver;
  class Networkproblem;
} // namespace Model

namespace Optimization {

  class ControlToStates {
  public:
    ControlToStates(Model::Timeevolver &timeevolver);

    bool get_states(
        Aux::InterpolatingVector const &controls,
        Aux::InterpolatingVector &states, Eigen::VectorXd const &initial_state,
        Model::Networkproblem &problem);

  private:
    Model::Timeevolver &evolver;

    std::pair<Aux::InterpolatingVector, Aux::InterpolatingVector> cache;
    Aux::InterpolatingVector last_failed;
  };

} // namespace Optimization
