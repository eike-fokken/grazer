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
#include "Edge.hpp"
#include "Node.hpp"
#include <string>

namespace Aux {
  template <typename T> static std::string component_class(T const &component) {
    bool constexpr is_node
        = (std::is_base_of_v<
            Network::Node,
            typename std::remove_reference<decltype(component)>::type>);
    bool constexpr is_edge
        = (std::is_base_of_v<
            Network::Edge,
            typename std::remove_reference<decltype(component)>::type>);
    static_assert(is_node or is_edge, "Component is neither node, nor edge!");

    if constexpr (is_node) {
      return "nodes";
    } else {
      return "connections";
    }
  }
} // namespace Aux
