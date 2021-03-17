#include "Power_factory.hpp"

#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  PowerFactory::PowerFactory() {
    std::vector<std::unique_ptr<AbstractNodeType>> node_types;

    node_types.push_back(std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    node_types.push_back(std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    node_types.push_back(std::make_unique<NodeType<Networkproblem::Power::PQnode>>());

    for (auto const &node_type : node_types) {
      this->node_type_map.insert({node_type->get_name(), node_type});
    }
  }

  std::map<std::string, Nodefactory> Power_factory::get_nodetypemap_impl(){

    std::map<std::string, Nodefactory> data;
        std::vector<std::unique_ptr<AbstractNodeType>> buildervector;

    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::PQnode>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_name(), builder->get_factory()});
    }

    return data;
  }
  std::map<std::string, Edgefactory> Power_factory::get_edgetypemap_impl(){
    std::map<std::string, Edgefactory> data;
    std::vector<std::unique_ptr<AbstractEdgeType>> buildervector;

    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Power::Transmissionline>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_name(), builder->get_factory()});
    }
    return data;
  }

}
