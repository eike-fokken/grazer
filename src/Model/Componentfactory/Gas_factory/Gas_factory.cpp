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
#include "Gas_factory.hpp"

#include "Compressorstation.hpp"
#include "ConstraintSink.hpp"
#include "Controlvalve.hpp"
#include "Innode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"

namespace Model::Componentfactory {

  void add_gas_components(
      Componentfactory &factory, nlohmann::json const &defaults) {
    factory.add_node_type(std::make_unique<NodeType<Gas::Source>>(defaults));
    factory.add_node_type(std::make_unique<NodeType<Gas::Sink>>(defaults));
    factory.add_node_type(std::make_unique<NodeType<Gas::Innode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Gas::ConstraintSink>>(defaults));

    factory.add_edge_type(std::make_unique<EdgeType<Gas::Pipe>>(defaults));
    factory.add_edge_type(std::make_unique<EdgeType<Gas::Shortpipe>>(defaults));
    factory.add_edge_type(
        std::make_unique<EdgeType<Gas::Controlvalve>>(defaults));
    factory.add_edge_type(
        std::make_unique<EdgeType<Gas::Compressorstation>>(defaults));
  }

  Gas_factory::Gas_factory(nlohmann::json const &defaults) {
    add_gas_components(*this, defaults);
  }
} // namespace Model::Componentfactory
