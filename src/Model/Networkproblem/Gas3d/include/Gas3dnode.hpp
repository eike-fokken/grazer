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
#include "Gas3dedge.hpp"
#include "Node.hpp"

namespace Model::Gas3d {

  class Gas3dnode : public Network::Node {

  public:
    using Node::Node;

    ~Gas3dnode() override;

    /// Claims outer indices of attached edges.
    void gasnode_setup_helper();

  protected:
    void evaluate_flow_node_balance(
        Eigen::Ref<Eigen::VectorXd> rootvalues,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        double prescribed_flow) const;

    void evaluate_flow_node_derivative(
        Aux::Matrixhandler &jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &state) const;

    std::vector<std::pair<Direction, Gas3dedge *>> directed_attached_gas_edges;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{0};
  };
} // namespace Model::Gas3d
