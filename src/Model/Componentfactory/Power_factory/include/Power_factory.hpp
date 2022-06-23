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

namespace Model::Componentfactory {

  /**
   * @brief add the Power Components to the provided Componentfactory
   *
   * @param factory to which to add the Power Components
   * @param defaults a json with defaults for the individual components
   */
  void add_power_components(
      Componentfactory &factory, nlohmann::json const &defaults);

  /** \brief This class provides a Componentfactory of all Power components.
   */
  struct Power_factory : public Componentfactory {
    /** \brief Constructs a factory of all power types
     *  \param defaults a json with defaults for the individual components
     */
    Power_factory(nlohmann::json const &defaults);
  };
} // namespace Model::Componentfactory
