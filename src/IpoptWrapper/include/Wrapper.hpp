#ifndef WRAPPER_H
#define WRAPPER_H

#include <Eigen/Dense>
#include <cassert>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#include <IpTNLP.hpp>

template <
    typename ObjFun, typename ObjGradFun, typename ConstrsFun,
    typename ConstrsJacFun>
class IpoptWrapper : public Ipopt::TNLP {
  static_assert(
      std::is_invocable_r_v<double, ObjFun, Eigen::Ref<Eigen::VectorXd const>>,
      "The objective function must be a callable with "
      "Eigen::VectorXd& argument and return a double");
  static_assert(
      std::is_invocable_r_v<
          Eigen::VectorXd, ObjGradFun, Eigen::Ref<Eigen::VectorXd const>>,
      "The gradient function must be a callable with "
      "Eigen::VectorXd& argument and return type Eigen::VectorXd");
  static_assert(
      std::is_invocable_r_v<
          Eigen::VectorXd, ConstrsFun, Eigen::Ref<Eigen::VectorXd const>>,
      "The constraints function must be a callable with "
      "Eigen::VectorXd& argument and return type Eigen::VectorXd");
  static_assert(
      std::is_invocable_r_v<
          std::tuple<Eigen::VectorXd, Eigen::VectorXi, Eigen::VectorXi>,
          ConstrsJacFun, Eigen::Ref<Eigen::VectorXd const>>,
      "The Jacobian function must be a callable with Eigen::VectorXd "
      "argument and return type std::tuple<Eigen::VectorXd, "
      "Eigen::VectorXi, Eigen::VectorXi>");

private:
  std::size_t _num_vars;
  std::size_t _num_constraints;
  Eigen::VectorXd _initial_point;
  Eigen::VectorXd _lower_bounds;
  Eigen::VectorXd _upper_bounds;
  Eigen::VectorXd _constr_lower_bounds;
  Eigen::VectorXd _constr_upper_bounds;
  //
  Eigen::VectorXd _x_sol;
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
  void set_initial_point(Eigen::VectorXd &x0) { _initial_point = x0; }
  Eigen::VectorXd get_initial_point() { return _initial_point; }
  std::size_t get_num_vars() { return _num_vars; }
  std::size_t get_num_constraints() { return _num_constraints; }
  Eigen::VectorXd get_solution() { return _x_sol; }
  double get_obj_value() { return _sol_obj_value; }

  /**
   * @brief Set the bounds on the variables, i.e. lower <= x <= upper.
   *
   * @param lower Vector of lower bounds for the variables.
   * @param upper Vector of upper bounds for the variables.
   */
  void set_variable_bounds(Eigen::VectorXd &lower, Eigen::VectorXd &upper) {
    assert(lower.size() == upper.size());
    _lower_bounds = lower;
    _upper_bounds = upper;
    _num_vars = static_cast<std::size_t>(lower.size());
  }

  /**
   * @brief Get a tuple of the lower and upper bounds on the variables
   *
   * @return std::tuple<Eigen::VectorXd, Eigen::VectorXd> Contains the lower and
   * upper bounds in that order.
   */
  std::tuple<Eigen::VectorXd, Eigen::VectorXd> get_variable_bounds() {
    return {_lower_bounds, _upper_bounds};
  }

  /**
   * @brief Set the lower and upper bounds on the constraint function, i.e.
   *        lower <= constraints(x) <= upper.
   *
   * @param lower Vector of lower bounds for the constraints.
   * @param upper Vector of upper bounds for the constraints.
   */
  void set_constraint_bounds(Eigen::VectorXd &lower, Eigen::VectorXd &upper) {
    assert(lower.size() == upper.size());
    _constr_lower_bounds = lower;
    _constr_upper_bounds = upper;
    _num_constraints = static_cast<std::size_t>(upper.size());
  }

  std::tuple<Eigen::VectorXd, Eigen::VectorXd> get_constraint_bounds() {
    return {_constr_lower_bounds, _constr_upper_bounds};
  }

  // Internal Ipopt methods

  /** @name Overloaded from TNLP */

  /** Method to return some info about the nlp */
  bool get_nlp_info(
      Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
      Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) final {
    n = static_cast<Ipopt::Index>(get_num_vars());
    m = static_cast<Ipopt::Index>(get_num_constraints());
    // Evaluate the constraints jacobian in the initial point to obtain the
    // number of nonzero values in the jacobian.
    auto x0 = get_initial_point();
    auto [nnz_constrs_jac, nnz_rows, nnz_cols] = _jacobian(x0);

    nnz_jac_g = static_cast<Ipopt::Index>(nnz_constrs_jac.size());
    nnz_h_lag = 0; // we don't use the hessian, so we can ignore it
    index_style = Ipopt::TNLP::IndexStyleEnum::C_STYLE; // 0-based indexing.
    return true;
  }

  /** Method to return the bounds for my problem */
  bool get_bounds_info(
      Ipopt::Index /*n*/, Ipopt::Number *x_l, Ipopt::Number *x_u,
      Ipopt::Index /*m*/, Ipopt::Number *g_l, Ipopt::Number *g_u) final {
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
      Ipopt::Index /* n */, bool /* init_x */, Ipopt::Number *x,
      bool /* init_z */, Ipopt::Number * /* z_L */, Ipopt::Number * /* z_U */,
      Ipopt::Index /*m */, bool /* init_lambda */,
      Ipopt::Number * /* lambda */) final {
    // initialize to the given starting point
    auto x0 = get_initial_point();
    std::copy(x0.cbegin(), x0.cend(), x);
    return true;
  }

  /** Method to return the objective value */
  bool eval_f(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
      Ipopt::Number &obj_value) final {
    Eigen::Map<Eigen::VectorXd const> xx(x, n);
    obj_value = _objective(xx);
    return true;
  }

  /** Method to return the gradient of the objective */
  bool eval_grad_f(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
      Ipopt::Number *grad_f) final {
    Eigen::Map<Eigen::VectorXd const> xx(x, n);
    auto grad = _gradient(xx);
    std::copy(grad.cbegin(), grad.cend(), grad_f);
    return true;
  }

  /** Method to return the constraint residuals */
  bool eval_g(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */, Ipopt::Index m,
      Ipopt::Number *g) final {
    Eigen::Map<Eigen::VectorXd const> xx(x, n);
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
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
      Ipopt::Index /* m */, Ipopt::Index /* nele_jac */, Ipopt::Index *iRow,
      Ipopt::Index *jCol, Ipopt::Number *values) final {
    if (values == NULL) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      auto x0 = get_initial_point();
      auto [jac_nnz_vals, nnz_rows, nnz_cols] = _jacobian(x0);
      std::copy(nnz_rows.cbegin(), nnz_rows.cend(), iRow);
      std::copy(nnz_cols.cbegin(), nnz_cols.cend(), jCol);
    } else {
      // evaluate the jacobian of the constraints function
      // Eigen::VectorXd xx(Eigen::Map<const Eigen::VectorXd>(x, n));
      Eigen::VectorXd xx(n);
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
      Ipopt::SolverReturn /* status */, Ipopt::Index n, const Ipopt::Number *x,
      const Ipopt::Number * /* z_L */, const Ipopt::Number * /* z_U */,
      Ipopt::Index /* m */, const Ipopt::Number * /* g */,
      const Ipopt::Number * /* lambda */, Ipopt::Number obj_value,
      const Ipopt::IpoptData * /* ip_data */,
      Ipopt::IpoptCalculatedQuantities * /* ip_cq */) final {
    Eigen::VectorXd xx(n);
    std::copy(x, x + n, xx.begin());
    _x_sol = xx;
    _sol_obj_value = obj_value;
  }
};

#endif /* End WRAPPER_H */
