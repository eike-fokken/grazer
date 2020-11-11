#include <Eigen/Sparse>
#include <eigen3/Eigen/src/SparseCore/SparseMatrix.h>

namespace Solver {

  struct Solutionobject {
    bool success{false};
    double residual{std::numeric_limits<double>::quiet_NaN()};
    int used_iterations{0};
  };

  class Newtonsolver {
  public:
    void update_jabobian();

    Solutionobject solve();

  private:
    constexpr static double const decrease_value{0.01};
    constexpr static double const minimal_stepsize{1e-10};

    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
        sparselusolver;
    Eigen::SparseMatrix<double> jacobian;
  };

} // namespace Solver
