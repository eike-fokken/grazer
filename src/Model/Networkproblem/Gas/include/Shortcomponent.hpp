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
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Gasedge.hpp"

namespace Model::Gas {

  class Shortcomponent : public Network::Edge, public Gasedge {

  public:
    using Edge::Edge;

    Eigen::Index needed_number_of_states() const final;
    static nlohmann::json get_initial_schema();

    void new_print_helper(
        nlohmann::json &new_output, std::string const &component_type,
        std::string const &type);

    void json_save(double, Eigen::Ref<Eigen::VectorXd const> const &) final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        const nlohmann::json &initial_json) const final;

    /// Shortcomponents just use pressure and volumetric flow as their
    /// variables, so that this function simply returns the corresponding
    /// boundary state.
    Eigen::Vector2d get_boundary_p_qvol_bar(
        Direction direction,
        Eigen::Ref<Eigen::VectorXd const> const &state) const final;

    /// Because Shortcomponents use pressure and volumetric flow as their
    /// state variables, this function just hands `function_derivative` to
    /// jacobianhandler.
    void dboundary_p_qvol_dstate(
        Direction direction, Aux::Matrixhandler &jacobianhandler,
        Eigen::RowVector2d function_derivative, Eigen::Index rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> const &state) const final;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr Eigen::Index number_of_state_variables{4};
  };

} // namespace Model::Gas
