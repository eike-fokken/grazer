#include <Subproblem.hpp>

namespace Model {

unsigned int Subproblem::reserve_indices(unsigned int next_free_index) {
  start_state_index = next_free_index;
  after_state_index = get_start_state_index() + get_number_of_states();
  
  return after_state_index;
}

  unsigned int Subproblem::get_start_state_index() {
    return start_state_index;
}
  unsigned int Subproblem::get_after_state_index() {
    return after_state_index;
  }

} // namespace Model
