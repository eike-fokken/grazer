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
#include "Controlcomponent.hpp"
namespace Model {

  void set_simple_control_values(
      Controlcomponent const *controlcomponent,
      Aux::InterpolatingVector_Base &full_control_vector,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema);

  class SimpleControlcomponent : public Controlcomponent {
  public:
    Eigen::Index set_control_indices(Eigen::Index next_free_index) final;

  private:
    /// \brief Returns number of control variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2.
    ///
    /// @returns number of control variables needed by this component
    virtual Eigen::Index needed_number_of_controls_per_time_point() const = 0;
  };
} // namespace Model
