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

#include "Balancelaw_factory.hpp"
#include "Exception.hpp"
#include "Isothermaleulerequation.hpp"
#include <memory>

namespace Model::Balancelaw {

  std::unique_ptr<Pipe_Balancelaw>
  make_pipe_balancelaw(nlohmann::json const &json) {
    if (json["balancelaw"] == "Isothermaleulerequation") {
      return std::make_unique<Isothermaleulerequation>(json);
    } else {
      gthrow({"Unknown type of Balancelaw!"});
    }
  }
} // namespace Model::Balancelaw
