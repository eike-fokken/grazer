#pragma once
#include "Cacheentry.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace Model {
  class Controlcomponent;
} // namespace Model

namespace Optimization {
  using SolverVector
      = std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>>;
  using MatrixVector = std::vector<Eigen::SparseMatrix<double>>;

  class EquationDerivativeCache final {
  public:
    EquationDerivativeCache();

    std::tuple<SolverVector const &, MatrixVector const &, MatrixVector const &>
    factorizations(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Model::Controlcomponent &problem);

  private:
    Cacheentry entry;
    std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>>
        dE_dnew_state_solvers;
    std::vector<Eigen::SparseMatrix<double>> dE_dlast_state;
    std::vector<Eigen::SparseMatrix<double>> dE_dcontrol;
  };

} // namespace Optimization
