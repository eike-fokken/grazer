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
#pragma once
#include <nlohmann/json.hpp>
#include <optional>
namespace Model {

  /** Base class for Equationcomponent and Controlcomponent
   *
   * Defines a setup function for any post construction intialization and the
   * possibility to define a boundary schema.
   */

  class Equation_base {
  public:
    virtual ~Equation_base() {}
    /** \brief Utility for setup of things that cannot be done during
     * construction.
     *
     * is called by Networkproblems reserve_state_indices.
     * Usually does nothing, but for example gas nodes
     * claim indices from their attached gas edges.
     */
    virtual void setup() = 0;
  };

} // namespace Model
