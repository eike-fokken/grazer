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

#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Networkproblem::Gas {

  class Sink final : public Flowboundarynode {

  public:
    static std::string get_type();

    Sink(nlohmann::json const &data);

  private:
    static nlohmann::json
    revert_boundary_conditions(nlohmann::json const &data);
  };

} // namespace Model::Networkproblem::Gas
