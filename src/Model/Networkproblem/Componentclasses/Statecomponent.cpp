#include "Statecomponent.hpp"
#include "Exception.hpp"

namespace Model {

  void Statecomponent::setup_output_json_helper(std::string const &id) {
    auto &output_json = get_output_json_ref();
    output_json["id"] = id;
    output_json["data"] = nlohmann::json::array();
  }

  int Statecomponent::get_number_of_states() const {
    return get_state_afterindex() - get_state_startindex();
  }

  int Statecomponent::get_state_startindex() const {
    if (state_startindex < 0) {
      gthrow(
          {"state_startindex < 0. Probably ", __func__, " was called ",
           "before calling set_indices().\n This is forbidden."});
    }
    return state_startindex;
  }
  int Statecomponent::get_state_afterindex() const {
    if (state_afterindex < 0) {
      gthrow(
          {"state_afterindex < 0. Probably ", __func__,
           " was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return state_afterindex;
  }

  nlohmann::json &Statecomponent::get_output_json_ref() {
    return component_output;
  }

} // namespace Model
