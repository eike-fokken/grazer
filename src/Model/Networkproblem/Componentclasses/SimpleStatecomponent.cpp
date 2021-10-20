#include "SimpleStatecomponent.hpp"
namespace Model {

  int SimpleStatecomponent::set_state_indices(int next_free_index) {
    startindex = next_free_index;
    int number_of_states = needed_number_of_states();
    afterindex = next_free_index + number_of_states;

    return afterindex;
  }

} // namespace Model
