
#include <Eigen/Dense>
#include <numeric>
#include <tuple>
#include <vector>

#include "Adaptor.hpp"
#include "IpIpoptApplication.hpp"
#include "Wrapper.hpp"
#include "gtest/gtest.h"

using VectorXd = Eigen::VectorXd;
using VectorXi = Eigen::VectorXi;

TEST(testHS071, hs071) {
  /*
  Our 'modern' optimization problem
  min x0 * x3 * (x0 + x1 + x2) + x2

  s.t.

  g1(x) = x0*x1*x2*x3 >= 25
  g2(x) = x0^2 + x1^2 + x2^2 + x3^2 = 40
  1 <= x0, x1, x2, x3 <= 5
  */

  auto objective = [](Eigen::Ref<VectorXd const> x) -> double {
    return x[0] * x[3] * (x[0] + x[1] + x[2]) + x[2];
  };

  auto obj_gradient = [](Eigen::Ref<VectorXd const> x) -> VectorXd {
    VectorXd ret(x.size());
    ret << x[0] * x[3] + x[3] * (x[0] + x[1] + x[2]), x[0] * x[3],
        x[0] * x[3] + 1.0, x[0] * (x[0] + x[1] + x[2]);
    return ret;
  };

  auto constraints = [](Eigen::Ref<VectorXd const> x) -> VectorXd {
    VectorXd ret(2);
    ret << x[0] * x[1] * x[2] * x[3],
        x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3];
    return ret;
  };

  auto jacobian = [](Eigen::Ref<VectorXd const> x)
      -> std::tuple<VectorXd, VectorXi, VectorXi> {
    /* the jacobian is dense:
    jac = ( g1_x1  g1_x2  g1_x3  g1_x4 )
          ( g2_x1  g2_x3  g2_x3  g2_x4 )
    */

    VectorXd jac_values(8);
    jac_values << x[1] * x[2] * x[3], // = g1_x1
        x[0] * x[2] * x[3],           // = g1_x2
        x[0] * x[1] * x[3],           // = g1_x3
        x[0] * x[1] * x[2],           // = g1_x4
        2 * x[0],                     // = g2_x1
        2 * x[1],                     // = g2_x2
        2 * x[2],                     // = g2_x3
        2 * x[3];                     // = g2_x4

    VectorXi nnz_rows(8), nnz_cols(8);
    nnz_rows << 0, 0, 0, 0, 1, 1, 1, 1;
    nnz_cols << 0, 1, 2, 3, 0, 1, 2, 3;

    return {jac_values, nnz_rows, nnz_cols};
  };

  VectorXd x0(4), x_lb(4), x_ub(4), constrs_lb(2), constrs_ub(2);
  x0 << 0.0, 0.0, 0.0, 0.0;   // initial point
  x_lb << 1.0, 1.0, 1.0, 1.0; // lower bound on the variables
  x_ub << 5.0, 5.0, 5.0, 5.0; // upper bound on the variables
  constrs_lb << 25, 40;       // bounds on the constraints
  constrs_ub << 2.0e19, 40;   // bounds on the constraints

  // Define the problee
  auto ipopt = IpoptAdaptor(objective, obj_gradient, constraints, jacobian);
  ipopt.set_initial_point(x0);
  ipopt.set_variable_bounds(x_lb, x_ub);
  ipopt.set_constraint_bounds(constrs_lb, constrs_ub);
  // Solve the problem
  auto status = ipopt.optimize();

  EXPECT_TRUE(status == 0);
  EXPECT_DOUBLE_EQ(ipopt.get_obj_value(), 1.7014017140214442e+01);
  auto sol = ipopt.get_solution();
  double abs_error = 1e-4;
  EXPECT_NEAR(sol[0], 1.0, abs_error);
  EXPECT_NEAR(sol[1], 4.74299964, abs_error);
  EXPECT_NEAR(sol[2], 3.82114998, abs_error);
  EXPECT_NEAR(sol[3], 1.3794083, abs_error);
}
