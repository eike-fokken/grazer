#include <Transmissionline.hpp>
#include <iostream>

namespace Model::Networkproblem::Power {

  double Transmissionline::get_G() const { return G; }

  double Transmissionline::get_B() const { return B; }

  void Transmissionline::display() const {
    Edge::print_id();
    std::cout << "type: TL, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
