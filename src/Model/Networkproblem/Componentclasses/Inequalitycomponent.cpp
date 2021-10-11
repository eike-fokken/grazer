#include "Inequalitycomponent.hpp"
#include "Exception.hpp"

namespace Model::Networkproblem {

  int Inequalitycomponent::get_number_of_inequalities() const {
    if (start_inequality_index < 0) {
      gthrow(
          {"start_inequality_index < 0. Probably get_number_of_inequalitys() "
           "was called "
           "before calling set_inequality_indices().\n This is forbidden."})
    }
    if (after_inequality_index < 0) {
      gthrow(
          {"after_inequality_index < 0. Probably get_number_of_inequalitys() "
           "was called "
           "before calling set_inequality_indices().\n This is forbidden."})
    }
    return after_inequality_index - start_inequality_index;
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
