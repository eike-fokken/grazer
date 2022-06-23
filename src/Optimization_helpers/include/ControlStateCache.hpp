#pragma once
#include "Cacheentry.hpp"
#include "Controlcomponent.hpp"
#include "InterpolatingVector.hpp"
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

namespace Model {
  class Timeevolver;
  class Controlcomponent;
} // namespace Model

namespace Optimization {

  class StateCache {
  public:
    virtual ~StateCache();
    virtual bool refresh_cache(
        Model::Controlcomponent &problem,
        Aux::InterpolatingVector_Base const &controls,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state)
        = 0;
    virtual Aux::InterpolatingVector_Base const &get_cached_states() = 0;
  };

  class ControlStateCache final : public StateCache {
  public:
    ControlStateCache(std::unique_ptr<Model::Timeevolver> evolver);

    bool refresh_cache(
        Model::Controlcomponent &problem,
        Aux::InterpolatingVector_Base const &controls,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state) final;

    Aux::InterpolatingVector_Base const &get_cached_states() final;

    Aux::InterpolatingVector_Base const *check_and_supply_states(
        Model::Controlcomponent &problem,
        Aux::InterpolatingVector_Base const &controls,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state);

  private:
    std::unique_ptr<Model::Timeevolver> evolver;

    std::pair<Cacheentry, Aux::InterpolatingVector> cache;
    Cacheentry failed;
  };

} // namespace Optimization
