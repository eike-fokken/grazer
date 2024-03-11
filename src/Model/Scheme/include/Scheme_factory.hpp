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
#include "Exception.hpp"
#include "Implicitboxscheme.hpp"
#include "Threepointscheme.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Scheme {

  template <int Dimension>
  std::unique_ptr<Scheme::Threepointscheme<Dimension>>
  make_threepointscheme(nlohmann::json const &json) {
    if (json["scheme"] == "Implicitboxscheme") {
      return std::make_unique<Implicitboxscheme<Dimension>>();
    } else {
      gthrow({"Unknown type of scheme!"});
    }
  }
} // namespace Model::Scheme
