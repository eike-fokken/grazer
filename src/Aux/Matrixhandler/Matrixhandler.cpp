#include <Eigen/Sparse>
#include <Matrixhandler.hpp>

namespace Aux {

  void Triplethandler::set_coefficient(unsigned int row, unsigned int col,
                                       double value) {
    Eigen::Triplet<double> newtriplet(static_cast<int>(row),
                                      static_cast<int>(col), value);
    tripletlist.push_back(newtriplet);
  }

  void Triplethandler::set_matrix() {
    matrix->setFromTriplets(tripletlist.begin(), tripletlist.end());
  }

  void Coeffrefhandler::set_coefficient(unsigned int row, unsigned int col,
                                        double value) {
    matrix->coeffRef(static_cast<int>(row), static_cast<int>(col)) = value;
  }

} // namespace Aux
