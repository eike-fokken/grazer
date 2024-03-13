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
#include "Equation_base.hpp"
#include <Eigen/Sparse>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>

namespace Aux {

  class Matrixhandler;
}

namespace Model {

  /** \brief A base class for network components that supply model equations to
   * be solved.
   *
   * An equation component owns a number of indices, namely between
   * #start_state_index (inclusive) and #after_state_index (exclusive), which
   * will be filled with results of the model equations of the component.
   * It is implicitely taken for granted that the set of owned indices of all
   * objects starts at 0 and is consecutive.
   */
  class Equationcomponent : public Equation_base {

  public:
    virtual ~Equationcomponent(){};

    /** \brief evaluates the model equations into rootvalues.
     *
     * @param[out] rootvalues Results of the model equations, when evaluated on
     * the other parameters.
     * @param last_time time point of the last time step. Usually important for
     * PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     */
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const
        = 0;

    /** \brief Carries out steps that need to be taken before the Newton method
     * for a time step can start.
     *
     * For most components does nothing.
     * @param last_time time point of the last time step. Usually important for
     * PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     */

    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state);

    /** \brief derivative of Equationcomponent::evaluate w.r.t. \p new_state.
     *
     * evaluates the derivative of Equationcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     */
    virtual void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const
        = 0;

    /** \brief derivative of Equationcomponent::evaluate w.r.t. \p last_state.
     *
     * evaluates the derivative of Equationcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix. The
     * default implementation is to do nothing because most components do not
     * depend on the last state.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     */

    virtual void d_evaluate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const
        = 0;
  };

} // namespace Model
