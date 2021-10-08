#include "Inequalitycomponent.hpp"

namespace Model::Networkproblem {

  /// \brief getter for #start_inequality_index
  int Inequalitycomponent::get_start_inequality_index() const {
    return start_inequality_index;
  }

  /// \brief getter for #after_inequality_index
  int Inequalitycomponent::get_after_inequality_index() const {
    return after_inequality_index;
  }
} // namespace Model::Networkproblem
