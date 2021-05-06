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
