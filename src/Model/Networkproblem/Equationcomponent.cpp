#include <Eigen/Sparse>
#include <Equationcomponent.hpp>

namespace Model::Networkproblem {

  int Equationcomponent::set_indices(int const next_free_index) {
    start_state_index = next_free_index;
    after_state_index = next_free_index + get_number_of_states();

    return after_state_index;
  }

  int Equationcomponent::get_start_state_index() const {
    return start_state_index;
  }
  int Equationcomponent::get_after_state_index() const {
    return after_state_index;
  }

} // namespace Model::Networkproblem
