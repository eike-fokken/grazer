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
namespace Model {

  /** \brief Interface class for components that depend on switches.
   *
   * This class only makes sense, if the implementing class also inherits from
   * Equationcomponent.
   */
  class Switchcomponent {
    // /** \brief SimpleControlcomponent is a friend of Controlcomponent to
    //  * give it access to #control_startindex and #control_afterindex.
    //  */
    // friend class SimpleControlcomponent;
    // friend class Networkproblem;
    // friend class ::Mock_OptimizableObject;
    // friend class ::TestControlComponent_for_ControlStateCache;

  public:
    static nlohmann::json get_switch_schema() = delete;

    virtual ~Switchcomponent(){};

    /** \brief This function sets the indices #start_switch_index and
     * #after_switch_index.
     *
     * @param next_free_index The first switch index that is currently not
     * claimed by another component.
     * @returns The new lowest free index.
     */
    virtual Eigen::Index set_switch_indices(Eigen::Index next_free_index) = 0;

    Eigen::Index get_number_of_switches_per_timepoint() const;

    /** \brief getter for #start_switch_index
     */
    Eigen::Index get_switch_startindex() const;

    /** \brief getter for #after_switch_index
     */
    Eigen::Index get_switch_afterindex() const;

    /** \brief Fills the indices owned by this component with switch values
     */

    virtual void set_initial_switches(
        Aux::InterpolatingVector_Base &full_switch_vector,
        nlohmann::json const &switch_json) const = 0;

    virtual void save_switches_to_json(
        Aux::InterpolatingVector_Base const &switches, nlohmann::json &json);

  private:
    /** These are also defined in Controlcomponent but are overridden by the
     * same functions.
     */
    virtual std::string componentclass() = 0;
    virtual std::string componenttype() = 0;
    virtual std::string id() = 0;

    /** \brief The first switch index, this Switchcomponent "owns".
     */
    Eigen::Index switch_startindex{-1};

    /** \brief The first switch index after #start_switch_index, that is
     * not "owned" by this Switchcomponent.
     */
    Eigen::Index switch_afterindex{-1};
  };
} // namespace Model
