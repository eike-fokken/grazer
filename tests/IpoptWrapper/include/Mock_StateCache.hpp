#pragma once
#include "ControlStateCache.hpp"
#include "InterpolatingVector.hpp"

namespace Model {
  class Timeevolver;
  class Controlcomponent;
} // namespace Model

namespace Optimization {

  class Mock_StateCache final : public StateCache {
  public:
    Mock_StateCache(Aux::InterpolatingVector_Base &_states) : states(_states) {}

    bool refresh_cache(
        Model::Controlcomponent & /*problem*/,
        Aux::InterpolatingVector_Base const & /*controls*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*state_timepoints*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*initial_state*/) final {
      return true;
    }

    Aux::InterpolatingVector_Base const &get_cached_states() final {
      return states;
    }

  private:
    Aux::InterpolatingVector states;
  };

} // namespace Optimization
