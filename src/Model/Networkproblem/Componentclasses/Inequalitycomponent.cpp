#include "Inequalitycomponent.hpp"

namespace Model::Networkproblem {

  int Inequalitycomponent::set_inequality_indices(int const next_free_index) {
    start_inequality_index = next_free_index;
    int number_of_inequalitys = get_number_of_inequalities();
    after_inequality_index = next_free_index + number_of_inequalitys;

    return after_inequality_index;
  }

  /// \brief getter for #start_inequality_index
  int Inequalitycomponent::get_start_inequality_index() const {
    return start_inequality_index;
  }

  /// \brief getter for #after_inequality_index
  int Inequalitycomponent::get_after_inequality_index() const {
    return after_inequality_index;
  }
} // namespace Model::Networkproblem
