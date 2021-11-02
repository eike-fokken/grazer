#include "ControlToStates.hpp"
#include "Exception.hpp"

namespace Optimization {

  ControlToStates::ControlToStates(nlohmann::json const &timeevolver_data) :
      evolver(Model::Timeevolver::make_instance(timeevolver_data)) {}

  bool ControlToStates::get_states(
      Aux::InterpolatingVector const &controls,
      Aux::InterpolatingVector &states) {
    if (last_failed == controls) {
      return false;
    }
    if (controls == cache.first) {
      states = cache.second;
      return true;
    } else {
      assert(false);
      gthrow({"nonono!"});
    }
  }
} // namespace Optimization
