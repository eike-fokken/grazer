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
#include "Switchcomponent.hpp"
namespace Model {

  void set_simple_switch_values(
      Switchcomponent const *switchcomponent,
      Aux::InterpolatingVector_Base &full_switch_vector,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema);

  class SimpleSwitchcomponent : public Switchcomponent {
  public:
    Eigen::Index set_switch_indices(Eigen::Index next_free_index) final;

    /** \brief getter for #start_switch_index
     */
    Eigen::Index get_switch_startindex() const final;

    /** \brief getter for #after_switch_index
     */
    Eigen::Index get_switch_afterindex() const final;

  private:
    /// \brief Returns number of switch variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2.
    ///
    /// @returns number of switch variables needed by this component
    virtual Eigen::Index needed_number_of_switches_per_time_point() const = 0;

    /** \brief The first switch index, this Switchcomponent "owns".
     */
    Eigen::Index switch_startindex{-1};

    /** \brief The first switch index after #start_switch_index, that is
     * not "owned" by this Switchcomponent.
     */
    Eigen::Index switch_afterindex{-1};
  };
} // namespace Model
