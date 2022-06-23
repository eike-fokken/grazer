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
