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

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add all components to the provided Componentfactory
   *
   * @param factory to which the components should be added
   */
  void add_all_components(Componentfactory &factory);

  /// \brief This class provides a Componentfactory of all Component types
  /// defined for the Networkproblem.
  ///
  /// At the moment these are Gas components and Power components.
  struct Full_factory : public Componentfactory {
    Full_factory();
  };
} // namespace Model::Componentfactory
