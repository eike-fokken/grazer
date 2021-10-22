#include <Eigen/Sparse>
#include <Matrixhandler.hpp>

namespace Aux {

  Matrixhandler::~Matrixhandler() {}

  void Triplethandler::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    Eigen::Triplet<double, Eigen::Index> newtriplet(row, col, value);
    tripletlist.push_back(newtriplet);
  }

  void Triplethandler::set_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    add_to_coefficient(row, col, value);
  }

  void Triplethandler::set_matrix() {
    matrix.setFromTriplets(tripletlist.begin(), tripletlist.end());
    tripletlist.clear();
  }

  void Coeffrefhandler::set_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    matrix.coeffRef(row, col) = value;
  }
  void Coeffrefhandler::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    matrix.coeffRef(row, col) += value;
  }

  void Coeffrefhandler::set_matrix() {}

} // namespace Aux
