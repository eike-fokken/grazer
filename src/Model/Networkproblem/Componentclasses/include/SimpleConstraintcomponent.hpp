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
#include "Constraintcomponent.hpp"
namespace Model {

  void set_simple_constraint_values(
      Constraintcomponent const *component,
      Aux::InterpolatingVector_Base &full_constraint_vector,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema);

  class SimpleConstraintcomponent : public Constraintcomponent {
  public:
    Eigen::Index set_constraint_indices(Eigen::Index next_free_index) final;

  private:
    /// \brief Returns number of constraint variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of constraint variables needed by this component
    virtual Eigen::Index
    needed_number_of_constraints_per_time_point() const = 0;
  };
} // namespace Model
