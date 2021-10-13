#include "Statecomponent.hpp"
#include "Exception.hpp"

namespace Model  {

  nlohmann::json Statecomponent::get_initial_schema() {
    gthrow(
        {"This static method must be implemented in the class inheriting "
         "from Equationcomponent!"});
  }

  void Statecomponent::setup_output_json_helper(std::string const &id) {
    auto &output_json = get_output_json_ref();
    output_json["id"] = id;
    output_json["data"] = nlohmann::json::array();
  }

  int Statecomponent::get_number_of_states() const {
    if (start_state_index < 0) {
      gthrow(
          {"start_state_index < 0. Probably get_number_of_states() was called "
           "before calling set_state_indices().\n This is forbidden."});
    }
    if (after_state_index < 0) {
      gthrow(
          {"after_state_index < 0. Probably get_number_of_states() was called "
           "before calling set_state_indices().\n This is forbidden."});
    }
    return after_state_index - start_state_index;
  }

  int Statecomponent::get_start_state_index() const {
    return start_state_index;
  }
  int Statecomponent::get_after_state_index() const {
    return after_state_index;
  }

  nlohmann::json &Statecomponent::get_output_json_ref() {
    return component_output;
  }

} // namespace Model 
