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
#include "SimpleStatecomponent.hpp"
#include <Eigen/Sparse>

namespace Model::Gas3d {

  enum Direction { start = 1, end = -1 };

  std::string Direction_string(Direction direction);
  /// @brief This class is a base class for all Gas3d edges with a 2x2 balance
  /// law
  class Gas3dedge : public SimpleStatecomponent {

  public:
    static int init_vals_per_interpol_point();

    virtual std::string get_gas_type() const = 0;
    Eigen::VectorXi boundary_equation_indices(Direction direction) const;

    Eigen::Index get_boundary_state_index(Direction direction) const;

    Eigen::Index get_equation_start_index() const;
    Eigen::Index get_equation_after_index() const;

    /// Returns the boundary
    Eigen::Vector3d get_boundary_state(
        Direction direction,
        Eigen::Ref<Eigen::VectorXd const> const &state) const;

  private:
    Eigen::Vector3d
    get_starting_state(Eigen::Ref<Eigen::VectorXd const> const &state) const;
    Eigen::Vector3d
    get_ending_state(Eigen::Ref<Eigen::VectorXd const> const &state) const;

    Eigen::VectorXi give_away_start_indices() const;
    Eigen::VectorXi give_away_end_indices() const;

    Eigen::Index get_starting_state_index() const;
    Eigen::Index get_ending_state_index() const;
  };

} // namespace Model::Gas3d
