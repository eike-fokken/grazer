/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "Wrapper.hpp"
#include "Optimizer.hpp"

#include <IpAlgTypes.hpp>
#include <IpTypes.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

namespace Optimization {

  IpoptWrapper::IpoptWrapper(std::unique_ptr<Optimizer> _optimizer) :
      optimizer(std::move(_optimizer)) {}

  IpoptWrapper::~IpoptWrapper() = default;
  bool IpoptWrapper::get_nlp_info(
      Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
      Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) {
    auto number_of_controls = optimizer->get_total_no_controls();
    auto number_of_constraints = optimizer->get_total_no_constraints();
    auto number_of_nonzeros_in_jacobian = optimizer->get_no_nnz_in_jacobian();
    if (number_of_controls > std::numeric_limits<Ipopt::Index>::max()) {
      std::cout << "Too many controls to fit into an Ipopt::Index!"
                << std::endl;
      return false;
    }
    if (number_of_constraints > std::numeric_limits<Ipopt::Index>::max()) {
      std::cout << "Too many constraints to fit into an Ipopt::Index!"
                << std::endl;
      return false;
    }
    if (number_of_nonzeros_in_jacobian
        > std::numeric_limits<Ipopt::Index>::max()) {
      std::cout
          << "Too many components in the constraint jacobian to fit into an "
             "Ipopt::Index!"
          << std::endl;
      return false;
    }
    n = static_cast<Ipopt::Index>(number_of_controls);
    m = static_cast<Ipopt::Index>(number_of_constraints);

    nnz_jac_g = static_cast<Ipopt::Index>(number_of_nonzeros_in_jacobian);
    nnz_h_lag = 0; // we don't use the hessian, so we can ignore it
    index_style = Ipopt::TNLP::IndexStyleEnum::C_STYLE; // 0-based indexing.

    return true;
  }
  bool IpoptWrapper::get_bounds_info(
      Ipopt::Index n, Ipopt::Number *x_l, Ipopt::Number *x_u, Ipopt::Index m,
      Ipopt::Number *g_l, Ipopt::Number *g_u) {

    assert(m == optimizer->get_total_no_constraints());

    Eigen::Map<Eigen::VectorXd> lower_bounds(x_l, n);
    lower_bounds = optimizer->get_lower_bounds();
    Eigen::Map<Eigen::VectorXd> upper_bounds(x_u, n);
    upper_bounds = optimizer->get_upper_bounds();

    Eigen::Map<Eigen::VectorXd> constraint_lower_bounds(g_l, m);
    constraint_lower_bounds = optimizer->get_constraint_lower_bounds();
    Eigen::Map<Eigen::VectorXd> constraint_upper_bounds(g_u, m);
    constraint_upper_bounds = optimizer->get_constraint_upper_bounds();

    return true;
  }
  bool IpoptWrapper::get_starting_point(
      Ipopt::Index n, bool init_x, Ipopt::Number *x, bool init_z,
      Ipopt::Number * /* z_L */, Ipopt::Number * /* z_U */, Ipopt::Index /*m */,
      bool init_lambda, Ipopt::Number * /* lambda */) {
    assert(init_x);
    assert(not init_z);
    assert(not init_lambda);

    // initialize to the given starting point
    Eigen::Map<Eigen::VectorXd> ipoptcontrols(x, n);
    ipoptcontrols = optimizer->get_initial_controls();

    return true;
  }
  bool IpoptWrapper::eval_f(
      Ipopt::Index number_of_controls, Ipopt::Number const *x, bool new_x,
      Ipopt::Number &objective_value) {
    if (new_x) {
      optimizer->new_x();
    }
    // Get controls into an InterpolatingVector_Base:
    Eigen::Map<Eigen::VectorXd const> controls(x, number_of_controls);

    return optimizer->evaluate_objective(controls, objective_value);
  }

  bool IpoptWrapper::eval_grad_f(
      Ipopt::Index number_of_controls, Ipopt::Number const *x, bool new_x,
      Ipopt::Number *grad_f) {
    if (new_x) {
      optimizer->new_x();
    }
    Eigen::Map<Eigen::VectorXd const> controls(x, number_of_controls);
    Eigen::Map<Eigen::VectorXd> gradient(grad_f, number_of_controls);
    return optimizer->evaluate_objective_gradient(controls, gradient);
  }
  bool IpoptWrapper::eval_g(
      Ipopt::Index number_of_controls, Ipopt::Number const *x, bool new_x,
      Ipopt::Index number_of_constraints, Ipopt::Number *g) {
    if (new_x) {
      optimizer->new_x();
    }
    Eigen::Map<Eigen::VectorXd const> controls(x, number_of_controls);
    Eigen::Map<Eigen::VectorXd> constraints(g, number_of_constraints);
    return optimizer->evaluate_constraints(controls, constraints);
  }
  bool IpoptWrapper::eval_jac_g(
      Ipopt::Index number_of_controls, Ipopt::Number const *x, bool new_x,
      Ipopt::Index /*number_of_constraints*/, Ipopt::Index nele_jac,
      Ipopt::Index *iRow, Ipopt::Index *jCol, Ipopt::Number *values) {

    if (values == nullptr) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      Eigen::Map<Eigen::VectorX<Ipopt::Index>> Rowindices(iRow, nele_jac);
      Eigen::Map<Eigen::VectorX<Ipopt::Index>> Colindices(jCol, nele_jac);
      return optimizer->supply_constraint_jacobian_indices(
          Rowindices, Colindices);
    }

    if (new_x) {
      optimizer->new_x();
    }

    Eigen::Map<Eigen::VectorXd const> controls(x, number_of_controls);
    Eigen::Map<Eigen::VectorXd> jacobian_values(values, nele_jac);

    return optimizer->evaluate_constraint_jacobian(controls, jacobian_values);
  }
  void IpoptWrapper::finalize_solution(
      Ipopt::SolverReturn status, Ipopt::Index number_of_controls,
      const Ipopt::Number *x, const Ipopt::Number * /* z_L */,
      const Ipopt::Number * /* z_U */, Ipopt::Index number_of_constraints,
      const Ipopt::Number *g, const Ipopt::Number * /* lambda */,
      Ipopt::Number obj_value, const Ipopt::IpoptData * /* ip_data */,
      Ipopt::IpoptCalculatedQuantities * /* ip_cq */) {
    if (status != Ipopt::SUCCESS) {
      std::cout << "\n\n\nFAILED TO FIND AN OPTIMUM:\nIpopt status was: "
                << static_cast<int>(Ipopt::SUCCESS) << "\n";
      std::cout << "Compare the values to the enum Ipopt::SUCCESS in the IPOPT "
                   "manual for what "
                   "that means.\n\n\n"
                << std::endl;
    }
    Eigen::Map<Eigen::VectorXd const> solution(x, number_of_controls);
    Eigen::Map<Eigen::VectorXd const> constraints(g, number_of_constraints);
    best_solution = solution;
    best_objective_value = obj_value;
    best_cost_value = 0;
    optimizer->evaluate_cost(solution, best_cost_value);
    best_penalty_value = 0;
    optimizer->evaluate_penalty(solution, best_penalty_value);
    best_constraints = constraints;
  }

  Eigen::VectorXd IpoptWrapper::get_best_solution() const {
    return best_solution;
  }
  double IpoptWrapper::get_best_objective_value() const {
    return best_objective_value;
  }
  double IpoptWrapper::get_best_cost_value() const { return best_cost_value; }
  double IpoptWrapper::get_best_penalty_value() const {
    return best_penalty_value;
  }
  Eigen::VectorXd IpoptWrapper::get_best_constraints() const {
    return best_constraints;
  }

} // namespace Optimization
