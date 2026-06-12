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
#include "Sink.hpp"
#include "Exception.hpp"
#include "make_schema.hpp"
#include <string>

namespace Model::Gas3d {

  nlohmann::json revert_first_boundary_conditions(nlohmann::json const &data) {
    nlohmann::json reverted_boundary_json = data;
    if (not reverted_boundary_json.contains("boundary_values")) {
      gthrow(
          {"The sink json ", data["id"],
           " did not contain boundary values.  Something went wrong.\n",
           "Its json was: ", data.dump(1, '\t')});
    }
    for (auto &datum : reverted_boundary_json["boundary_values"]["data"]) {
      auto &value_json = datum["values"];
      value_json[0] = -value_json[0].get<double>();
    }

    return reverted_boundary_json;
  }
  std::string Sink::get_type() { return "Sink"; }

  Sink::Sink(nlohmann::json const &data) :
      Flowboundarynode(data),
      boundaryvalue(
          Aux::InterpolatingVector::construct_from_json(
              data["boundary_values"], get_boundary_schema())) {}

  /// \brief Boundary values are inflow and inflow share of gas 1.
  nlohmann::json Sink::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  double Sink::prescribed_flow_value(double time) const {
    auto boundary_value = boundaryvalue(time)[0];
    if (boundary_value > 0) {
      gthrow(
          {"Gas sink outflow is less than 0 in Sink with id:\n", get_id(),
           "\nat time:\n", std::to_string(time), "This is not feasible!"});
    }
    return boundaryvalue(time)[0];
  }
  double Sink::prescribed_component_1_share(double) const { return 1; }

} // namespace Model::Gas3d
