#include "Statecomponent.hpp"

namespace Model::Networkproblem {

  void Statecomponent::setup_helper(std::string const &id) {
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

  void Statecomponent::push_to_values(
      double t, std::vector<std::map<double, double>> value_vector) {
    (values_ptr->times).push_back(t);
    (values_ptr->values).push_back(value_vector);
  }

  std::vector<double> const &Statecomponent::get_times() const {
    return values_ptr->times;
  }
  std::vector<std::vector<std::map<double, double>>> const &
  Statecomponent::get_values() const {
    return values_ptr->values;
  }

  nlohmann::json &Statecomponent::get_output_json_ref() {
    return component_output;
  }

  bool Statecomponent::needs_output_file() { return true; }

} // namespace Model::Networkproblem
