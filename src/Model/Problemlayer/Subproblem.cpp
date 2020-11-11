#include <Exception.hpp>
#include <Subproblem.hpp>

namespace Model {

  unsigned int Subproblem::get_number_of_states() const {
    int int_start = static_cast<int>(start_state_index);
    int int_after = static_cast<int>(after_state_index);

    // Be careful: We expect number of states to be GREATER than zero.
    if (int number_of_states = int_after - int_start > 0) {
      return static_cast<unsigned int>(number_of_states);
    } else {
      gthrow("Number of states is negative or zero!");
    }
  }

  unsigned int Subproblem::set_indices(unsigned int const next_free_index) {
    start_state_index = next_free_index;
    after_state_index = reserve_indices(next_free_index);
    return after_state_index;
  }

  unsigned int Subproblem::get_start_state_index() const {
    return start_state_index;
  }
  unsigned int Subproblem::get_after_state_index() const {
    return after_state_index;
  }

} // namespace Model
