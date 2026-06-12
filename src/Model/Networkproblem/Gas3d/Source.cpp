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
#include "Source.hpp"
#include "Exception.hpp"
#include "Flowboundarynode.hpp"
#include "make_schema.hpp"
namespace Model::Gas3d {

  std::string Source::get_type() { return "Source"; }

  Source::Source(nlohmann::json const &data) :
      Flowboundarynode(data),
      boundaryvalue(
          Aux::InterpolatingVector::construct_from_json(
              data["boundary_values"], get_boundary_schema())) {}

  /// \brief Boundary values are inflow and inflow share of gas 1.
  nlohmann::json Source::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(2);
  }

  double Source::prescribed_flow_value(double time) const {
    auto boundary_value = boundaryvalue(time)[0];
    if (boundary_value < 0) {

      gthrow(
          {"Gas source outflow is less than 0 in Source with id:\n", get_id(),
           "\nat time:\n", std::to_string(time), "This is not feasible!"});
    }
    return boundaryvalue(time)[0];
  }
  double Source::prescribed_component_1_share(double time) const {
    return boundaryvalue(time)[1];
  }

} // namespace Model::Gas3d
