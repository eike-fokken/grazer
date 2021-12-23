#include <Eigen/Sparse>
#include <Matrixhandler.hpp>

namespace Aux {
  Matrixhandler::~Matrixhandler() {}

  template <int Transpose>
  void Triplethandler<Transpose>::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    Eigen::Triplet<double, Eigen::Index> newtriplet;
    if constexpr (not Transpose) {
      newtriplet = {row, col, value};
    } else {
      newtriplet = {col, row, value};
    }
    tripletlist.push_back(newtriplet);
  }
  template <int Transpose>
  void Triplethandler<Transpose>::set_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    if constexpr (not Transpose) {
      add_to_coefficient(row, col, value);
    } else {
      add_to_coefficient(col, row, value);
    }
  }
  template <int Transpose> void Triplethandler<Transpose>::set_matrix() {
    matrix.setFromTriplets(tripletlist.begin(), tripletlist.end());
    tripletlist.clear();
  }

  template <int Transpose>
  void Coeffrefhandler<Transpose>::set_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    if constexpr (not Transpose) {
      matrix.coeffRef(row, col) = value;
    } else {
      matrix.coeffRef(col, row) = value;
    }
  }
  template <int Transpose>
  void Coeffrefhandler<Transpose>::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    if constexpr (not Transpose) {
      matrix.coeffRef(row, col) += value;
    } else {
      matrix.coeffRef(col, row) += value;
    }
  }

  template <int Transpose> void Coeffrefhandler<Transpose>::set_matrix() {}

  template class Triplethandler<Transposed>;
  template class Triplethandler<Regular>;

  template class Coeffrefhandler<Transposed>;
  template class Coeffrefhandler<Regular>;

} // namespace Aux
