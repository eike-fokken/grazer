#include "Statecomponent.hpp"
#include "Exception.hpp"

namespace Model {

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
    return get_afterindex() - get_startindex();
  }

  int Statecomponent::get_startindex() const {
    if (startindex < 0) {
      gthrow(
          {"startindex < 0. Probably get_startindex() was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return startindex;
  }
  int Statecomponent::get_afterindex() const {
    if (afterindex < 0) {
      gthrow(
          {"afterindex < 0. Probably get_afterindex() was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return afterindex;
  }

  nlohmann::json &Statecomponent::get_output_json_ref() {
    return component_output;
  }

} // namespace Model
