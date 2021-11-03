#pragma once
#include "InterpolatingVector.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace Model {
  class Timeevolver;
  class Controlcomponent;
} // namespace Model

namespace Optimization {

  class ControlToStates {
  public:
    ControlToStates(
        Model::Timeevolver &timeevolver, Model::Controlcomponent &problem,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state);

    bool get_states(
        Aux::InterpolatingVector const &controls,
        Aux::InterpolatingVector &states);

  private:
    Model::Timeevolver &evolver;
    Model::Controlcomponent &problem;
    Eigen::VectorXd const initial_state;

    std::pair<Aux::InterpolatingVector, Aux::InterpolatingVector> cache;
    Aux::InterpolatingVector last_failed;
  };

} // namespace Optimization
