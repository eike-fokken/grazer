
#include <numeric>
#include <tuple>
#include <vector>

#include "Adaptor.hpp"
#include "IpIpoptApplication.hpp"
#include "Wrapper.hpp"
#include "gtest/gtest.h"

using VectorXd = Eigen::VectorXd;
using VectorXi = Eigen::VectorXi;

TEST(testUnconstrained, unconstrained) {
  /* min x^2 - 1
   */

  auto objective
      = [](Eigen::Ref<VectorXd const> x) -> double { return x[0] * x[0] - 1; };
  auto grad = [](Eigen::Ref<VectorXd const> x) -> VectorXd {
    VectorXd ret(x.size());
    ret << 2 * x[0];
    return ret;
  };
  auto constrs = [](Eigen::Ref<VectorXd const> x) -> VectorXd {
    VectorXd ret(x.size());
    ret << 0.0;
    return ret;
  };
  auto jacobian = [](Eigen::Ref<VectorXd const> /*x*/)
      -> std::tuple<VectorXd, VectorXi, VectorXi> {
    VectorXd jac_nnz_values(1);
    VectorXi nnz_rows(1), nnz_cols(1);
    jac_nnz_values << 0.0;
    nnz_rows << 0;
    nnz_cols << 0;
    return {jac_nnz_values, nnz_rows, nnz_cols};
  };

  // problem data
  VectorXd x0(1), x_lb(1), x_ub(1), con_lb(1), con_ub(1);
  x0 << 1.0;
  x_lb << -2.0e19;
  x_ub << 2.0e19;
  con_lb << -2.0e19;
  con_ub << 2.0e19;

  // Define the problee
  auto ipopt = IpoptAdaptor(objective, grad, constrs, jacobian);
  ipopt.set_initial_point(x0);
  ipopt.set_variable_bounds(x_lb, x_ub);
  ipopt.set_constraint_bounds(con_lb, con_ub);
  // Solve the problem
  auto status = ipopt.optimize();

  EXPECT_TRUE(status == 0);
  EXPECT_DOUBLE_EQ(ipopt.get_obj_value(), -1.0);
  auto sol = ipopt.get_solution();
  double abs_error = 1e-4;
  EXPECT_NEAR(sol[0], 0.0, abs_error);
}
