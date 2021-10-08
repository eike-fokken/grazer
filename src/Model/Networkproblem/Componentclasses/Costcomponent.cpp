#include "Costcomponent.hpp"

namespace Model::Networkproblem {
  Costcomponent::Costcomponent(double _cost_weight) :
      cost_weight(_cost_weight) {}

  double Costcomponent::get_cost_weight() { return cost_weight; }

} // namespace Model::Networkproblem
