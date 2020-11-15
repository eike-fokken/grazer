#include <Eigen/Sparse>
#include <Matrixhandler.hpp>

namespace Aux {

  void Triplethandler::add_to_coefficient(int row, int col, double value) {
    Eigen::Triplet<double> newtriplet(row, col, value);
    tripletlist.push_back(newtriplet);
  }

  void Triplethandler::set_coefficient(int row, int col, double value) {
    add_to_coefficient(row, col, value);
  }

  void Triplethandler::set_matrix() {
    matrix->setFromTriplets(tripletlist.begin(), tripletlist.end());
  }

  void Coeffrefhandler::set_coefficient(int row, int col, double value) {
    matrix->coeffRef(row, col) = value;
  }
  void Coeffrefhandler::add_to_coefficient(int row, int col, double value) {
    matrix->coeffRef(row, col) += value;
  }

} // namespace Aux
