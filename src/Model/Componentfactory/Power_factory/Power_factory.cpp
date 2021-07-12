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

#include "Power_factory.hpp"

#include "PQnode.hpp"

#include "PVnode.hpp"
#include "StochasticPQnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_power_components(Componentfactory &factory) {
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::PQnode>>());
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::StochasticPQnode>>());

    factory.add_edge_type(
        std::make_unique<EdgeType<Networkproblem::Power::Transmissionline>>());
  }

  Power_factory::Power_factory() { add_power_components(*this); }
} // namespace Model::Componentfactory
