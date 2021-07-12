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

#include "Shortpipe.hpp"
#include "Exception.hpp"
#include "Get_base_component.hpp"
#include "Matrixhandler.hpp"
#include <fstream>
#include <iostream>

namespace Model::Networkproblem::Gas {

  std::string Shortpipe::get_type() { return "Shortpipe"; }
  std::string Shortpipe::get_gas_type() const { return get_type(); }

  void Shortpipe::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    rootvalues.segment<2>(get_equation_start_index())
        = get_boundary_state(1, new_state) - get_boundary_state(-1, new_state);
  }

  void Shortpipe::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const>) const {

    auto start_p_index = get_boundary_state_index(1);
    auto start_q_index = start_p_index + 1;
    auto end_p_index = get_boundary_state_index(-1);
    auto end_q_index = end_p_index + 1;

    auto start_equation_index = get_equation_start_index();
    auto end_equation_index = start_equation_index + 1;

    jacobianhandler->set_coefficient(start_equation_index, start_p_index, 1.0);
    jacobianhandler->set_coefficient(start_equation_index, end_p_index, -1.0);
    jacobianhandler->set_coefficient(end_equation_index, start_q_index, 1.0);
    jacobianhandler->set_coefficient(end_equation_index, end_q_index, -1.0);
  }

  void Shortpipe::print_to_files(nlohmann::json &new_output) {
    std::string comp_type = Aux::component_class(*this);
    new_print_helper(new_output, comp_type, get_type());
  }

  void Shortpipe::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {
    initial_values_helper(new_state, initial_json);
  }

} // namespace Model::Networkproblem::Gas
