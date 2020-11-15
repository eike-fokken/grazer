#include <Powernode.hpp>

namespace Model::Networkproblem::Power {

  int Powernode::get_number_of_states() const { return 2; }

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

} // namespace Model::Networkproblem::Power
