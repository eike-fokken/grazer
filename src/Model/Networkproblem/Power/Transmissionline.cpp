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
#include "Powernode.hpp"
#include "make_schema.hpp"
#include <Exception.hpp>
#include <Transmissionline.hpp>
#include <iostream>
#include <tuple>

namespace Model::Power {

  nlohmann::json Transmissionline::get_schema() {
    nlohmann::json schema = Network::Edge::get_schema();
    Aux::schema::add_required(schema, "B", Aux::schema::type::number());
    Aux::schema::add_required(schema, "G", Aux::schema::type::number());

    return schema;
  }

  std::string Transmissionline::get_type() { return "Transmissionline"; }

  Transmissionline::Transmissionline(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Edge(topology, nodes), G(topology["G"]), B(topology["B"]) {}

  double Transmissionline::get_G() const { return G; }

  double Transmissionline::get_B() const { return B; }

  Powernode *Transmissionline::get_starting_powernode() const {
    return dynamic_cast<Powernode *>(get_starting_node());
  }

  Powernode *Transmissionline::get_ending_powernode() const {
    return dynamic_cast<Powernode *>(get_ending_node());
  }

} // namespace Model::Power
