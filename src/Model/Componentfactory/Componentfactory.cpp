#include <Componentfactory.hpp>

namespace model::Componentfactory {
  struct Componentfactory {
    std::map<std::string, AbstractNodeType> get_node_type_map() {
      return node_type_map;
    }
    std::map<std::string, AbstractEdgeType> get_edge_type_map() {
      return edge_type_map;
    }

  }
}