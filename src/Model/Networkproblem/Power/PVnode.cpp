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

#include <Matrixhandler.hpp>
#include <PVnode.hpp>
#include <Transmissionline.hpp>
#include <iostream>

namespace Model::Networkproblem::Power {

  std::string PVnode::get_type() { return "PVnode"; }
  std::string PVnode::get_power_type() const { return get_type(); }

  void PVnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];

    rootvalues[phi_index] = new_state[V_index] - boundaryvalue(new_time)[1];
  }

  void PVnode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    evaluate_P_derivative(V_index, jacobianhandler, new_state);
    jacobianhandler->set_coefficient(phi_index, V_index, 1.0);
  }

  void PVnode::json_save(double time, Eigen::Ref<Eigen::VectorXd const> state) {
    auto P_val = boundaryvalue(time)[0];
    auto Q_val = Q(state);
    json_save_power(time, state, P_val, Q_val);
  }

} // namespace Model::Networkproblem::Power
