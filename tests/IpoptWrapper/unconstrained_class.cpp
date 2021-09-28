
#include <numeric>
#include <tuple>
#include <vector>

#include "Adaptor.hpp"
#include "IpIpoptApplication.hpp"
#include "Wrapper.hpp"
#include "gtest/gtest.h"

using VectorXd = Eigen::VectorXd;
using VectorXi = Eigen::VectorXi;

class Problem {
public:
  double static objective(Eigen::Ref<VectorXd const> x) {
    return x[0] * x[0] - 1;
  }
  VectorXd static grad(Eigen::Ref<VectorXd const> x) {
    VectorXd ret(x.size());
    ret << 2.0 * x[0];
    return ret;
  }
  VectorXd static constrs(Eigen::Ref<VectorXd const> x) {
    VectorXd ret(x.size());
    ret << 0.0;
    return ret;
  }
  std::tuple<VectorXd, VectorXi, VectorXi> static jacobian(
      Eigen::Ref<VectorXd const> x) {
    VectorXd jac_nnz_values(1);
    VectorXi nnz_rows(1), nnz_cols(1);
    jac_nnz_values << 0.0;
    nnz_rows << 0.0;
    nnz_cols << 0.0;
    return {jac_nnz_values, nnz_rows, nnz_cols};
  }
};

TEST(testUnconstrained_class, unconstrained_class) {
  // problem data
  Problem op;
  VectorXd x0(1), x_lb(1), x_ub(1), con_lb(1), con_ub(1);
  x0 << 1.0;
  x_lb << -2.0e19;
  x_ub << 2.0e19;
  con_lb << -2.0e19;
  con_ub << 2.e19;

  // Define the problee
  auto ipopt = IpoptAdaptor(&op.objective, &op.grad, &op.constrs, &op.jacobian);
  ipopt.set_initial_point(x0);
  ipopt.set_variable_bounds(x_lb, x_ub);
  ipopt.set_constraint_bounds(con_lb, con_ub);
  // Solve the problem
  auto status = ipopt.optimize();

  // Tests
  EXPECT_TRUE(status == 0);
  EXPECT_DOUBLE_EQ(ipopt.get_obj_value(), -1.0);
  auto sol = ipopt.get_solution();
  double abs_error = 1e-4;
  EXPECT_NEAR(sol[0], 0.0, abs_error);
}