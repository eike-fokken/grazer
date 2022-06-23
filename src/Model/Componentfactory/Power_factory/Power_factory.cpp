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
#include "Power_factory.hpp"

#include "PQnode.hpp"

#include "PVnode.hpp"
#include "StochasticPQnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_power_components(
      Componentfactory &factory, nlohmann::json const &defaults) {
    factory.add_node_type(
        std::make_unique<NodeType<Power::Vphinode>>(defaults));
    factory.add_node_type(std::make_unique<NodeType<Power::PVnode>>(defaults));
    factory.add_node_type(std::make_unique<NodeType<Power::PQnode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Power::StochasticPQnode>>(defaults));

    factory.add_edge_type(
        std::make_unique<EdgeType<Power::Transmissionline>>(defaults));
  }

  Power_factory::Power_factory(nlohmann::json const &defaults) {
    add_power_components(*this, defaults);
  }
} // namespace Model::Componentfactory
