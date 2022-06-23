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

#include "Componentfactory.hpp"
#include <nlohmann/json.hpp>

namespace Model::Componentfactory {

  /**
   * @brief add Gas Components to the provided Componentfactory
   * @param factory to which the Gas Component schould be added
   * @param defaults a json with defaults for the individual components
   */
  void
  add_gas_components(Componentfactory &factory, nlohmann::json const &defaults);

  /** \brief This class provides a Componentfactory of all Gas components.
   */
  struct Gas_factory : public Componentfactory {

    /** \brief Constructs a factory of all gas component types.
     * \param defaults a json with defaults for the individual components
     */
    Gas_factory(nlohmann::json const &defaults);
  };
} // namespace Model::Componentfactory
