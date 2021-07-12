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
#include "Powernode.hpp"
#include <Edge.hpp>
#include <memory>
#include <string>

namespace Model::Networkproblem::Power {

  class Transmissionline final : public Network::Edge {

  public:
    static std::string get_type();
    static nlohmann::json get_schema();

    Transmissionline(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    double get_G() const;
    double get_B() const;

    /// Returns a pointer to the starting powernode.
    /// The constructor makes sure that this cast is valid.
    Powernode *get_starting_powernode() const;

    /// Returns a pointer to the ending powernode.
    /// The constructor makes sure that this cast is valid.
    Powernode *get_ending_powernode() const;

  private:
    double G;
    double B;
  };

} // namespace Model::Networkproblem::Power
