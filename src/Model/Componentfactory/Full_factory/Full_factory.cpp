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

  void add_all_components(Componentfactory &factory) {
    add_power_components(factory);
    add_gas_components(factory);
    factory.add_edge_type(
        std::make_unique<EdgeType<
            Networkproblem::Gaspowerconnection::Gaspowerconnection>>());
    factory.add_node_type(
        std::make_unique<NodeType<
            Networkproblem::Gaspowerconnection::ExternalPowerplant>>());
  }

  Full_factory::Full_factory() { add_all_components(*this); }
} // namespace Model::Componentfactory
