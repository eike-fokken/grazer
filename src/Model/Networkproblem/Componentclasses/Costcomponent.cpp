#include "Costcomponent.hpp"

namespace Model {
  Costcomponent::Costcomponent() {}
  Costcomponent::Costcomponent(double _cost_weight) :
      cost_weight(_cost_weight) {}

  double Costcomponent::get_cost_weight() const { return cost_weight; }

} // namespace Model
