#include "SimpleInequalitycomponent.hpp"
namespace Model::Networkproblem {

  int SimpleInequalitycomponent::set_inequality_indices(int next_free_index) {
    start_inequality_index = next_free_index;
    int number_of_inequalities = needed_number_of_inequalities();
    after_inequality_index = next_free_index + number_of_inequalities;

    return after_inequality_index;
  }

} // namespace Model::Networkproblem
