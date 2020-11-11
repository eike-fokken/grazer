#include <Eigen/Sparse>
#include <Equationcomponent.hpp>

namespace Model::Networkproblem {

  unsigned int
  Equationcomponent::set_indices(unsigned int const next_free_index) {
    start_state_index = next_free_index;
    after_state_index = next_free_index + get_number_of_states();

    return after_state_index;
  }

  unsigned int Equationcomponent::get_start_state_index() const {
    return start_state_index;
  }
  unsigned int Equationcomponent::get_after_state_index() const {
    return after_state_index;
  }

} // namespace Model::Networkproblem
