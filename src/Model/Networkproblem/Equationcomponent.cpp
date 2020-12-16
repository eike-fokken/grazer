#include <Eigen/Sparse>
#include <Equationcomponent.hpp>
#include <Exception.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem {

  void Equationcomponent::setup() {}

  int Equationcomponent::set_indices(int const next_free_index) {
    start_state_index = next_free_index;
    int number_of_states = get_number_of_states();
    after_state_index = next_free_index + number_of_states;

    return after_state_index;
  }

  int Equationcomponent::get_start_state_index() const {
    return start_state_index;
  }
  int Equationcomponent::get_after_state_index() const {
    return after_state_index;
  }

  void Equationcomponent::print_indices() const {
    std::cout << "start: " << start_state_index
              << ", end: " << after_state_index << ", ";
  }

  void Equationcomponent::push_to_values(
      double t, std::vector<std::map<double, double>> value_vector) {
    eqtimes.push_back(t);
    eqvalues.push_back(value_vector);
  }

  std::vector<double> const &Equationcomponent::get_times() const {
    return eqtimes;
  }
  std::vector<std::vector<std::map<double, double>>> const &
  Equationcomponent::get_values() const {
    return eqvalues;
  }

} // namespace Model::Networkproblem
