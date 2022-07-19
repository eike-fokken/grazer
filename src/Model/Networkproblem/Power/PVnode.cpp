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
#include <Matrixhandler.hpp>
#include <PVnode.hpp>
#include <Transmissionline.hpp>
#include <iostream>

namespace Model::Power {

  std::string PVnode::get_type() { return "PVnode"; }
  std::string PVnode::get_power_type() const { return get_type(); }

  void PVnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];

    rootvalues[phi_index] = new_state[V_index] - boundaryvalue(new_time)[1];
  }

  void PVnode::setup() { Powernode::setup_helper(); }

  void PVnode::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    evaluate_P_derivative(V_index, jacobianhandler, new_state);
    jacobianhandler.add_to_coefficient(phi_index, V_index, 1.0);
  }

  void PVnode::d_evalutate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

  void PVnode::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    auto P_val = boundaryvalue(time)[0];
    auto Q_val = Q(state);
    json_save_power(time, state, P_val, Q_val);
  }

} // namespace Model::Power
