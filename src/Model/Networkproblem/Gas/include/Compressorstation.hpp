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
#include "Shortpipe.hpp"

namespace Model::Networkproblem::Gas {

  class Compressorstation final : public Shortpipe {
  public:
    using Shortpipe::Shortpipe;
    static std::string get_type();
    std::string get_gas_type() const override;
    static std::optional<nlohmann::json> get_control_schema();
    void print_to_files(nlohmann::json &new_output) override;
  };
} // namespace Model::Networkproblem::Gas
