#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <map>

namespace Model::Networkproblem {

  int Equationcomponent::get_dimension_of_pde(){
    gthrow(
        {"This static method must be implemented in the class inheriting "
         "from Equationcomponent!"});
  }

  nlohmann::json Equationcomponent::get_initial_schema() {
    gthrow(
        {"This static method must be implemented in the class inheriting "
         "from Equationcomponent!"});
  }

  bool Equationcomponent::needs_output_file() { return true; }

  void Equationcomponent::prepare_timestep(
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> // new_state
  ) {}

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

  void Equationcomponent::push_to_values(
      double t, std::vector<std::map<double, double>> value_vector) {
    (values_ptr->times).push_back(t);
    (values_ptr->values).push_back(value_vector);
  }

  std::vector<double> const &Equationcomponent::get_times() const {
    return values_ptr->times;
  }
  std::vector<std::vector<std::map<double, double>>> const &
  Equationcomponent::get_values() const {
    return values_ptr->values;
  }

} // namespace Model::Networkproblem
