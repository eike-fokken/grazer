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

#include <Innode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  std::string Innode::get_type() { return "Innode"; }

  void Innode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    evaluate_flow_node_balance(rootvalues, new_state, 0.0);
  }
  void Innode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

} // namespace Model::Networkproblem::Gas
