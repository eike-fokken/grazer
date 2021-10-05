#ifndef WRAPPER_H
#define WRAPPER_H

#include <Eigen/Dense>
#include <cassert>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#include "IpTNLP.hpp"

using VectorXd = Eigen::VectorXd;
using VectorXi = Eigen::VectorXi;

using namespace Ipopt;

template <
    typename ObjFun, typename ObjGradFun, typename ConstrsFun,
    typename ConstrsJacFun>
class IpoptWrapper : public TNLP {
  static_assert(
      std::is_invocable_r_v<double, ObjFun, Eigen::Ref<VectorXd const>>,
      "The objective function must be a callable with "
      "VectorXd& argument and return a double");
  static_assert(
      std::is_invocable_r_v<VectorXd, ObjGradFun, Eigen::Ref<VectorXd const>>,
      "The gradient function must be a callable with "
      "VectorXd& argument and return type VectorXd");
  static_assert(
      std::is_invocable_r_v<VectorXd, ConstrsFun, Eigen::Ref<VectorXd const>>,
      "The constraints function must be a callable with "
      "VectorXd& argument and return type VectorXd");
  static_assert(
      std::is_invocable_r_v<
          std::tuple<VectorXd, VectorXi, VectorXi>, ConstrsJacFun,
          Eigen::Ref<VectorXd const>>,
      "The Jacobian function must be a callable with VectorXd "
      "argument and return type std::tuple<VectorXd, "
      "VectorXi, VectorXi>");

private:
  std::size_t _num_vars;
  std::size_t _num_constraints;
  VectorXd _initial_point;
  VectorXd _lower_bounds;
  VectorXd _upper_bounds;
  VectorXd _constr_lower_bounds;
  VectorXd _constr_upper_bounds;
  //
  VectorXd _x_sol;
  double _sol_obj_value;

  // Functions
  ObjFun _objective;
  ObjGradFun _gradient;
  ConstrsFun _constraints;
  ConstrsJacFun _jacobian;

public:
  IpoptWrapper(
      ObjFun objective, ObjGradFun gradient, ConstrsFun constraints,
      ConstrsJacFun jacobian) :
      _objective(objective),
      _gradient(gradient),
      _constraints(constraints),
      _jacobian(jacobian){};
  ~IpoptWrapper() = default;
  IpoptWrapper(const IpoptWrapper &) = delete;
  IpoptWrapper &operator=(const IpoptWrapper &) = delete;

  // Setters and getters
  void set_initial_point(VectorXd &x0) { _initial_point = x0; }
  VectorXd get_initial_point() { return _initial_point; }
  std::size_t get_num_vars() { return _num_vars; }
  std::size_t get_num_constraints() { return _num_constraints; }
  VectorXd get_solution() { return _x_sol; }
  double get_obj_value() { return _sol_obj_value; }

  /**
   * @brief Set the bounds on the variables, i.e. lower <= x <= upper.
   *
   * @param lower
   * @param upper
   */
  void set_variable_bounds(VectorXd &lower, VectorXd &upper) {
    assert(lower.size() == upper.size());
    _lower_bounds = lower;
    _upper_bounds = upper;
    _num_vars = static_cast<std::size_t>(lower.size());
  }

  /**
   * @brief Get a tuple of the lower and upper bounds on the variables
   *
   * @return std::tuple<VectorXd, VectorXd>
   */
  std::tuple<VectorXd, VectorXd> get_variable_bounds() {
    return {_lower_bounds, _upper_bounds};
  }

  /**
   * @brief Set the lower and upper bounds on the constraint function, i.e.
   *        lower <= constraints(x) <= upper.
   *
   * @param lower
   * @param upper
   */
  void set_constraint_bounds(VectorXd &lower, VectorXd &upper) {
    assert(lower.size() == upper.size());
    _constr_lower_bounds = lower;
    _constr_upper_bounds = upper;
    _num_constraints = static_cast<std::size_t>(upper.size());
  }

  std::tuple<VectorXd, VectorXd> get_constraint_bounds() {
    return {_constr_lower_bounds, _constr_upper_bounds};
  }

  // Internal Ipopt methods

  /**@name Overloaded from TNLP */
  //@{
  /** Method to return some info about the nlp */
  bool get_nlp_info(
      Index &n, Index &m, Index &nnz_jac_g, Index &nnz_h_lag,
      IndexStyleEnum &index_style) override {
    n = static_cast<Ipopt::Index>(get_num_vars());
    m = static_cast<Ipopt::Index>(get_num_constraints());
    // Evaluate the constraints jacobian in the initial point to obtain the
    // number of nonzero values in the jacobian.
    auto x0 = get_initial_point();
    auto [nnz_constrs_jac, nnz_rows, nnz_cols] = _jacobian(x0);

    nnz_jac_g = static_cast<Ipopt::Index>(nnz_constrs_jac.size());
    nnz_h_lag = 0; // we don't use the hessian, so we can ignore it
    index_style = TNLP::IndexStyleEnum::C_STYLE; // 0-based indexing.
    return true;
  }

  /** Method to return the bounds for my problem */
  bool get_bounds_info(
      Index n, Number *x_l, Number *x_u, Index m, Number *g_l,
      Number *g_u) override {
    n = static_cast<Ipopt::Index>(get_num_vars());
    m = static_cast<Ipopt::Index>(get_num_constraints());
    auto [vars_lb, vars_ub] = get_variable_bounds();
    auto [constrs_lb, constrs_ub] = get_constraint_bounds();
    std::copy(vars_lb.cbegin(), vars_lb.cend(), x_l);
    std::copy(vars_ub.cbegin(), vars_ub.cend(), x_u);
    std::copy(constrs_lb.cbegin(), constrs_lb.cend(), g_l);
    std::copy(constrs_ub.cbegin(), constrs_ub.cend(), g_u);
    return true;
  }

  /** Method to return the starting point for the algorithm */
  bool get_starting_point(
      Index /* n */, bool /* init_x */, Number *x, bool /* init_z */,
      Number * /* z_L */, Number * /* z_U */, Index /*m */,
      bool /* init_lambda */, Number * /* lambda */) override {
    // initialize to the given starting point
    auto x0 = get_initial_point();
    std::copy(x0.cbegin(), x0.cend(), x);
    return true;
  }

  /** Method to return the objective value */
  bool eval_f(
      Index n, const Number *x, bool /* new_x */, Number &obj_value) override {
    Eigen::Map<VectorXd const> xx(x, n);
    obj_value = _objective(xx);
    return true;
  }

  /** Method to return the gradient of the objective */
  bool eval_grad_f(
      Index n, const Number *x, bool /* new_x */, Number *grad_f) override {
    Eigen::Map<VectorXd const> xx(x, n);
    auto grad = _gradient(xx);
    std::copy(grad.cbegin(), grad.cend(), grad_f);
    return true;
  }

  /** Method to return the constraint residuals */
  bool eval_g(
      Index n, const Number *x, bool /* new_x */, Index m, Number *g) override {
    Eigen::Map<VectorXd const> xx(x, n);
    auto constrs = _constraints(xx);
    assert(constrs.size() == m);
    std::copy(constrs.cbegin(), constrs.cend(), g);
    return true;
  }

  /** Method to return:
   *   1) The structure of the Jacobian (if "values" is NULL)
   *   2) The values of the Jacobian (if "values" is not NULL)
   */
  bool eval_jac_g(
      Index n, const Number *x, bool /* new_x */, Index /* m */,
      Index /* nele_jac */, Index *iRow, Index *jCol, Number *values) override {
    if (values == NULL) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      auto x0 = get_initial_point();
      auto [jac_nnz_vals, nnz_rows, nnz_cols] = _jacobian(x0);
      std::copy(nnz_rows.cbegin(), nnz_rows.cend(), iRow);
      std::copy(nnz_cols.cbegin(), nnz_cols.cend(), jCol);
    } else {
      // evaluate the jacobian of the constraints function
      // VectorXd xx(Eigen::Map<const VectorXd>(x, n));
      VectorXd xx(n);
      std::copy(x, x + n, xx.begin());
      auto [jac_nnz_vals, nnz_rows, nnz_cols] = _jacobian(xx);
      std::copy(jac_nnz_vals.cbegin(), jac_nnz_vals.cend(), values);
    }
    return true;
  }

  /** Method to return:
   *   1) The structure of the Hessian of the Lagrangian (if "values" is NULL)
   *   2) The values of the Hessian of the Lagrangian (if "values" is not
   * NULL)
   */
  // virtual bool eval_h(Index n, const Number* x, bool new_x, Number
  // obj_factor,
  //                     Index m, const Number* lambda, bool new_lambda,
  //                     Index nele_hess, Index* iRow, Index* jCol,
  //                     Number* values);

  /** This method is called when the algorithm is complete so the TNLP can
   * store/write the solution */
  void finalize_solution(
      SolverReturn /* status */, Index n, const Number *x,
      const Number * /* z_L */, const Number * /* z_U */, Index /* m */,
      const Number * /* g */, const Number * /* lambda */, Number obj_value,
      const IpoptData * /* ip_data */,
      IpoptCalculatedQuantities * /* ip_cq */) override {
    VectorXd xx(n);
    std::copy(x, x + n, xx.begin());
    _x_sol = xx;
    _sol_obj_value = obj_value;
  }
};

#endif /* End WRAPPER_H */
