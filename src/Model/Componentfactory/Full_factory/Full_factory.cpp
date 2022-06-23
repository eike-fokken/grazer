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
#include "Full_factory.hpp"
#include "ExternalPowerplant.hpp"
#include "Gas_factory.hpp"
#include "Power_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_all_components(
      Componentfactory &factory, nlohmann::json const &defaults) {
    add_power_components(factory, defaults);
    add_gas_components(factory, defaults);
    factory.add_edge_type(
        std::make_unique<EdgeType<Gaspowerconnection::Gaspowerconnection>>(
            defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Gaspowerconnection::ExternalPowerplant>>(
            defaults));
  }

  Full_factory::Full_factory(nlohmann::json const &defaults) {
    add_all_components(*this, defaults);
  }
} // namespace Model::Componentfactory
