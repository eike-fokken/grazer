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

#include "Compressorstation.hpp"

#include "Get_base_component.hpp"
#include "make_schema.hpp"

namespace Model::Networkproblem::Gas {

  std::string Compressorstation::get_type() { return "Compressorstation"; }
  std::string Compressorstation::get_gas_type() const { return get_type(); }

  std::optional<nlohmann::json> Compressorstation::get_control_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  void Compressorstation::print_to_files(nlohmann::json &new_output) {
    std::string comp_type = Aux::component_class(*this);
    new_print_helper(new_output, comp_type, get_type());
  }

} // namespace Model::Networkproblem::Gas
