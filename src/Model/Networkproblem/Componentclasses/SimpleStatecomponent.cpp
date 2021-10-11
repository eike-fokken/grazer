#include "SimpleStatecomponent.hpp"
namespace Model::Networkproblem {

  int SimpleStatecomponent::set_state_indices(int next_free_index) {
    start_state_index = next_free_index;
    int number_of_states = needed_number_of_states();
    after_state_index = next_free_index + number_of_states;

    return after_state_index;
  }

} // namespace Model::Networkproblem
