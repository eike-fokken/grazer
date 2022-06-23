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
#pragma once
#include <Eigen/Dense>
#include <IpTNLP.hpp>
#include <limits>
#include <memory>

namespace Optimization {

  class Optimizer;

  class IpoptWrapper final : public Ipopt::TNLP {

  public:
    IpoptWrapper(std::unique_ptr<Optimizer> optimizer);

    ~IpoptWrapper() final;

    // Ipopt method overrides:

    /** Method to return some info about the nlp */
    bool get_nlp_info(
        Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
        Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) final;

    /** Method to return the bounds for my problem */
    bool get_bounds_info(
        Ipopt::Index n, Ipopt::Number *x_l, Ipopt::Number *x_u, Ipopt::Index m,
        Ipopt::Number *g_l, Ipopt::Number *g_u) final;

    /** Method to return the starting point for the algorithm */
    bool get_starting_point(
        Ipopt::Index n, bool init_x, Ipopt::Number *x, bool init_z,
        Ipopt::Number *z_L, Ipopt::Number *z_U, Ipopt::Index m,
        bool init_lambda, Ipopt::Number *lambda) final;

    /** Method to return the objective value */
    bool eval_f(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x,
        Ipopt::Number &obj_value) final;

    /** Method to return the gradient of the objective */
    bool eval_grad_f(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x,
        Ipopt::Number *grad_f) final;

    /** Method to return the constraint residuals */
    bool eval_g(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x, Ipopt::Index m,
        Ipopt::Number *g) final;

    /** Method to return:
     *   1) The structure of the Jacobian (if "values" is NULL)
     *   2) The values of the Jacobian (if "values" is not NULL)
     */
    bool eval_jac_g(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x, Ipopt::Index m,
        Ipopt::Index nele_jac, Ipopt::Index *iRow, Ipopt::Index *jCol,
        Ipopt::Number *values) final;

    /** This method is called when the algorithm is complete so the TNLP can
     * store/write the solution
     */
    void finalize_solution(
        Ipopt::SolverReturn status, Ipopt::Index n, const Ipopt::Number *x,
        const Ipopt::Number *z_L, const Ipopt::Number *z_U, Ipopt::Index m,
        const Ipopt::Number *g, const Ipopt::Number *lambda,
        Ipopt::Number obj_value, const Ipopt::IpoptData *ip_data,
        Ipopt::IpoptCalculatedQuantities *ip_cq) final;

    Eigen::VectorXd get_best_solution() const;
    double get_best_objective_value() const;
    Eigen::VectorXd get_best_constraints() const;

  private:
    std::unique_ptr<Optimizer> optimizer;

    Eigen::VectorXd best_solution;
    double best_objective_value = std::numeric_limits<double>::max();
    Eigen::VectorXd best_constraints;
  };

} // namespace Optimization
