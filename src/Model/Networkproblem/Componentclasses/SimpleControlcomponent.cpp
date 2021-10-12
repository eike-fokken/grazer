#include "SimpleControlcomponent.hpp"
namespace Model::Networkproblem {

  int SimpleControlcomponent::set_control_indices(int next_free_index) {
    start_control_index = next_free_index;
    int number_of_controls = needed_number_of_controls_per_time_point();
    after_control_index = next_free_index + number_of_controls;

    return after_control_index;
  }

} // namespace Model::Networkproblem
