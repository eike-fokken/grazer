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

#include "Gas_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Innode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"

namespace Model::Componentfactory {

  void add_gas_components(Componentfactory &factory) {
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    factory.add_edge_type(
        std::make_unique<EdgeType<Networkproblem::Gas::Pipe>>());
    factory.add_edge_type(
        std::make_unique<EdgeType<Networkproblem::Gas::Shortpipe>>());
    factory.add_edge_type(
        std::make_unique<EdgeType<Networkproblem::Gas::Controlvalve>>());
    factory.add_edge_type(
        std::make_unique<EdgeType<Networkproblem::Gas::Compressorstation>>());
  }

  Gas_factory::Gas_factory() { add_gas_components(*this); }
} // namespace Model::Componentfactory
