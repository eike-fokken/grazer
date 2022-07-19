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
#include "Shortpipe.hpp"
#include "Exception.hpp"
#include "Get_base_component.hpp"
#include "Matrixhandler.hpp"
#include <fstream>
#include <iostream>

namespace Model::Gas {

  std::string Shortpipe::get_type() { return "Shortpipe"; }
  std::string Shortpipe::get_gas_type() const { return get_type(); }

  void Shortpipe::setup() { setup_output_json_helper(get_id()); }

  void Shortpipe::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    rootvalues.segment<2>(get_equation_start_index())
        = get_boundary_state(start, new_state)
          - get_boundary_state(end, new_state);
  }

  void Shortpipe::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {

    auto start_p_index = get_boundary_state_index(start);
    auto start_q_index = start_p_index + 1;
    auto end_p_index = get_boundary_state_index(end);
    auto end_q_index = end_p_index + 1;

    auto start_equation_index = get_equation_start_index();
    auto end_equation_index = start_equation_index + 1;

    jacobianhandler.add_to_coefficient(start_equation_index, start_p_index, 1.0);
    jacobianhandler.add_to_coefficient(start_equation_index, end_p_index, -1.0);
    jacobianhandler.add_to_coefficient(end_equation_index, start_q_index, 1.0);
    jacobianhandler.add_to_coefficient(end_equation_index, end_q_index, -1.0);
  }

  void Shortpipe::d_evalutate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

  void Shortpipe::add_results_to_json(nlohmann::json &new_output) {
    std::string comp_type = Aux::component_class(*this);
    new_print_helper(new_output, comp_type, get_type());
  }

} // namespace Model::Gas
