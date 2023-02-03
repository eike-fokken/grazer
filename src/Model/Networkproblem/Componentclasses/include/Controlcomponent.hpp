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
#include "Equationcomponent.hpp"
#include "Timedata.hpp"
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector_Base;
  class Matrixhandler;
} // namespace Aux
class Mock_OptimizableObject;
class TestControlComponent_for_ControlStateCache;
namespace Model {

  /** \brief Interface class for components that depend on controls.
   *
   * This class only makes sense, if the implementing class also inherits from
   * Equationcomponent.
   */
  class Controlcomponent : public Equationcomponent {
    /** \brief SimpleControlcomponent is a friend of Controlcomponent to
     * give it access to #control_startindex and #control_afterindex.
     */
    friend class SimpleControlcomponent;
    friend class Networkproblem;
    friend class ::Mock_OptimizableObject;
    friend class ::TestControlComponent_for_ControlStateCache;

  public:
    static nlohmann::json get_control_schema() = delete;

    virtual ~Controlcomponent(){};

    /** \brief derivative of Equationcomponent::evaluate w.r.t. \p control.
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
     * @param control value of the control at current time step.
     */
    virtual void d_evaluate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief This function sets the indices #start_control_index and
     * #after_control_index.
     *
     * @param next_free_index The first control index that is currently not
     * claimed by another component.
     * @returns The new lowest free index.
     */
    virtual Eigen::Index set_control_indices(Eigen::Index next_free_index) = 0;

    Eigen::Index get_number_of_controls_per_timepoint() const;

    /** \brief getter for #start_control_index
     */
    Eigen::Index get_control_startindex() const;

    /** \brief getter for #after_control_index
     */
    Eigen::Index get_control_afterindex() const;

    /** \brief Fills the indices owned by this component with control values
     */

    virtual void set_initial_controls(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &control_json) const = 0;

    virtual void set_lower_bounds(
        Aux::InterpolatingVector_Base &full_lower_bound_vector,
        nlohmann::json const &lower_bound_json) const = 0;

    virtual void set_upper_bounds(
        Aux::InterpolatingVector_Base &full_upper_bound_vector,
        nlohmann::json const &upper_bound_json) const = 0;

    virtual void save_controls_to_json(
        Aux::InterpolatingVector_Base const &controls, nlohmann::json &json);

  private:
    virtual std::string componentclass() = 0;
    virtual std::string componenttype() = 0;
    virtual std::string id() = 0;
    /** \brief The first control index, this Controlcomponent "owns".
     */
    Eigen::Index control_startindex{-1};

    /** \brief The first control index after #start_control_index, that is
     * not "owned" by this Controlcomponent.
     */
    Eigen::Index control_afterindex{-1};
  };
} // namespace Model
