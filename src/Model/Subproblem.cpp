#include <Subproblem.hpp>


namespace Model {


  int Subproblem::reserve_indices(int next_free_index)
  {
    start_continuous_state_index=next_free_index;
    after_continuous_state_index=get_after_continuous_state_index();

    return after_continuous_state_index;
  }

  void Subproblem::set_sporadic_state_indices(std::vector<int> indices)
  {
    sporadic_state_indices=indices;
  }

} // namespace Model
