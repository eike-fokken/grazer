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
  class Equationcomponent {

  public:
    virtual ~Equationcomponent(){};

    /** \brief evaluates the model equations into rootvalues. Note that this
     * should only actually depend on the control vector, if Controlcomponent is
     * also a base class of the implementing class.
     *
     * @param[out] rootvalues Results of the model equations, when evaluated
     * on the other parameters.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief Carries out steps that need to be taken before the Newton
     * method for a time step can start.
     *
     * For most components does nothing.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param control value of the control at current time step.
     */

    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /** \brief derivative of Controlcomponent::evaluate w.r.t. \p new_state.
     *
     * evaluates the derivative of Controlcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief derivative of Controlcomponent::evaluate w.r.t. \p
     * last_state.
     *
     * evaluates the derivative of Controlcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void d_evaluate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief Utility for setup of things that cannot be done during
     * construction.
     *
     * is called by Networkproblems reserve_state_indices.
     * Usually does nothing, but for example gas nodes
     * claim indices from their attached gas edges.
     */
    virtual void setup() = 0;
  };

} // namespace Model
