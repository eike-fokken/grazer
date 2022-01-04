#include "Matrixhandler.hpp"
#include <Eigen/Sparse>

namespace Aux {
  Matrixhandler::~Matrixhandler() {}

  template <int Transpose>
  Triplethandler<Transpose>::Triplethandler(
      Eigen::SparseMatrix<double> &_matrix) :
      Matrixhandler(_matrix) {
    assert(_matrix.nonZeros() == 0);
  }

  template <int Transpose>
  void Triplethandler<Transpose>::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    Eigen::Index actual_row = -1;
    Eigen::Index actual_col = -1;
    if constexpr (not Transpose) {
      actual_row = row;
      actual_col = col;
    } else {
      actual_row = col;
      actual_col = row;
    }
    assert(0 <= actual_row);
    assert(actual_row < matrix.rows());
    assert(0 <= actual_col);
    assert(actual_col < matrix.cols());
    Eigen::Triplet<double, Eigen::Index> newtriplet
        = {actual_row, actual_col, value};
    tripletlist.push_back(newtriplet);
  }
  template <int Transpose>
  void Triplethandler<Transpose>::set_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    // set_coefficient will take care of swapping row and col, therefore no
    // difference here in set_coefficient!
    add_to_coefficient(row, col, value);
  }
  template <int Transpose> void Triplethandler<Transpose>::set_matrix() {
    matrix.setFromTriplets(tripletlist.begin(), tripletlist.end());
    tripletlist.clear();
  }

  template <int Transpose>
  Coeffrefhandler<Transpose>::Coeffrefhandler(
      Eigen::SparseMatrix<double> &_matrix) :
      Matrixhandler(_matrix) {
    // This assert wrongly fails if the matrix actually contains no zeros.
    //  assert(_matrix.nonZeros() != 0);
  }

  template <int Transpose>
  void Coeffrefhandler<Transpose>::set_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    Eigen::Index actual_row = -1;
    Eigen::Index actual_col = -1;
    if constexpr (not Transpose) {
      actual_row = row;
      actual_col = col;
    } else {
      actual_row = col;
      actual_col = row;
    }
    assert(0 <= actual_row);
    assert(actual_row < matrix.rows());
    assert(0 <= actual_col);
    assert(actual_col < matrix.cols());
    matrix.coeffRef(actual_row, actual_col) = value;
  }
  template <int Transpose>
  void Coeffrefhandler<Transpose>::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {

    Eigen::Index actual_row = -1;
    Eigen::Index actual_col = -1;
    if constexpr (not Transpose) {
      actual_row = row;
      actual_col = col;
    } else {
      actual_row = col;
      actual_col = row;
    }
    assert(0 <= actual_row);
    assert(actual_row < matrix.rows());
    assert(0 <= actual_col);
    assert(actual_col < matrix.cols());
    matrix.coeffRef(actual_row, actual_col) += value;
  }

  template <int Transpose> void Coeffrefhandler<Transpose>::set_matrix() {}

  template class Triplethandler<Transposed>;
  template class Triplethandler<Regular>;

  template class Coeffrefhandler<Transposed>;
  template class Coeffrefhandler<Regular>;

} // namespace Aux
