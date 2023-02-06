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
#include "Statecomponent.hpp"
namespace Model {

  Eigen::VectorXd
  identity_converter(Eigen::Ref<Eigen::VectorXd const> const &x);

  void set_simple_initial_values(
      Statecomponent const *component,
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema,
      Eigen::Index number_of_points = 1, double Delta_x = 1.0,
      std::function<Eigen::VectorXd(Eigen::Ref<Eigen::VectorXd const> const &)>
          converter_function
      = identity_converter);

  class SimpleStatecomponent : public Statecomponent {
  public:
    Eigen::Index set_state_indices(Eigen::Index next_free_index) final;

    Eigen::Index get_state_startindex() const final;

    Eigen::Index get_state_afterindex() const final;

  private:
    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component
    virtual Eigen::Index needed_number_of_states() const = 0;

    /// \brief The first index, this Equationcomponent "owns".
    ///
    /// Most equation components write only to their own indices between
    /// #start_state_index (inclusive) and #after_state_index (exclusive).
    /// There are exceptions though, e.g. instances of \ref
    /// Model::Gas::Gasnode "Gasnode".
    Eigen::Index state_startindex{-1};

    /// \brief The first index after #start_state_index, that is not "owned" by
    /// this Equationcomponent.
    ///
    /// Most equation components write only to their own indices between
    /// #start_state_index (inclusive) and #after_state_index (exclusive).
    /// There are exceptions though, e.g. instances of
    /// \ref Model::Gas::Gasnode "Gasnode".
    Eigen::Index state_afterindex{-1};
  };
} // namespace Model
