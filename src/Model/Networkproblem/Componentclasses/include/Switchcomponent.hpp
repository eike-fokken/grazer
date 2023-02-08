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

    /** \brief getter for the start of this components switch indices.
     *
     * If you override this function, it should probably be the same as in e.g.
     * #SimpleSwitchcomponent and you probably need a data member Eigen::Index
     * switch_startindex.
     */
    virtual Eigen::Index get_switch_startindex() const = 0;

    /** \brief getter for the first index after this components switch indices.
     *
     * If you override this function, it should probably be the same as in e.g.
     * #SimpleSwitchcomponent and you probably need a data member Eigen::Index
     * switch_afterindex.
     */
    virtual Eigen::Index get_switch_afterindex() const = 0;

    /** \brief Fills the indices owned by this component with switch values
     */

    virtual void set_initial_switches(
        Aux::InterpolatingVector_Base &full_switch_vector,
        nlohmann::json const &switch_json) const = 0;

    virtual void save_switches_to_json(
        Aux::InterpolatingVector_Base const &switches, nlohmann::json &json);

  private:
    /** \brief Returns the high-level class of component, "nodes" or
     * "connections".
     *
     * This is needed to write jsons, that have separate entries for nodes and
     * edges.
     */
    virtual std::string componentclass() const = 0;
    /** \brief Returns the type of component, "nodes" or "connections".
     *
     * This is needed to write jsons.
     */
    virtual std::string componenttype() const = 0;
    virtual std::string id() const = 0;
  };
} // namespace Model
