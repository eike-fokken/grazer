#include "Statecomponent.hpp"

namespace Model::Networkproblem {

  void Statecomponent::setup_output_json_helper(std::string const &id) {
    auto &output_json = get_output_json_ref();
    output_json["id"] = id;
    output_json["data"] = nlohmann::json::array();
  }

  int Statecomponent::set_indices(int const next_free_index) {
    start_state_index = next_free_index;
    int number_of_states = get_number_of_states();
    after_state_index = next_free_index + number_of_states;

    return after_state_index;
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

  void Statecomponent::print_to_files(nlohmann::json &){};
} // namespace Model::Networkproblem
