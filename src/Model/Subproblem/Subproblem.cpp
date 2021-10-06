#include <Exception.hpp>
#include <Subproblem.hpp>

namespace Model {

  int Subproblem::get_number_of_states() const {
    int int_start = start_state_index;
    int int_after = after_state_index;

    // We expect number of states to be GREATER than zero.
    if (int number_of_states = int_after - int_start > 0) {
      return number_of_states;
    } else {
      gthrow({"Number of states is negative or zero!"});
    }
  }

  int Subproblem::get_number_of_controls() const {
    int int_start = start_control_index;
    int int_after = after_control_index;

    // We expect number of controls to be GREATER OR EQUAL to zero.
    if (int number_of_controls = int_after - int_start >= 0) {
      return number_of_controls;
    } else {
      gthrow({"Number of controls is negative!"});
    }
  }

  int Subproblem::set_state_indices(int const next_free_index) {
    start_state_index = next_free_index;
    after_state_index = reserve_state_indices(next_free_index);
    return after_state_index;
  }

  int Subproblem::set_control_indices(int const next_free_index) {
    start_control_index = next_free_index;
    after_control_index = reserve_control_indices(next_free_index);
    return after_control_index;
  }

  int Subproblem::get_start_state_index() const { return start_state_index; }
  int Subproblem::get_after_state_index() const { return after_state_index; }

  int Subproblem::get_start_control_index() const {
    return start_control_index;
  }
  int Subproblem::get_after_control_index() const {
    return after_control_index;
  }

} // namespace Model
