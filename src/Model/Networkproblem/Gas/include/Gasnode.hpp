/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#pragma once
#include "Equationcomponent.hpp"
#include "Gasedge.hpp"
#include "Node.hpp"

namespace Model::Networkproblem::Gas {

  class Gasnode : public Equationcomponent, public Network::Node {

  public:
    using Node::Node;

    /// Claims outer indices of attached edges.
    void setup() override;

  protected:
    void evaluate_flow_node_balance(
        Eigen::Ref<Eigen::VectorXd> rootvalues,
        Eigen::Ref<Eigen::VectorXd const> state, double prescribed_flow) const;

    void evaluate_flow_node_derivative(
        Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> state) const;

    std::vector<std::pair<int, Gasedge *>> directed_attached_gas_edges;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{0};
  };
} // namespace Model::Networkproblem::Gas
