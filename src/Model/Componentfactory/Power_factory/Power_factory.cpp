#include "Power_factory.hpp"

#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  std::map<std::string, Nodefactory> Power_factory::get_nodetypemap_impl(){

    std::map<std::string, Nodefactory> data;
        std::vector<std::unique_ptr<AbstractNodeType>> buildervector;

    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::PQnode>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_type(), builder->get_factory()});
    }

    return data;
  }
  std::map<std::string, Edgefactory> Power_factory::get_edgetypemap_impl(){
    std::map<std::string, Edgefactory> data;
    std::vector<std::unique_ptr<Edgedatabuilder_base>> buildervector;

    buildervector.push_back(std::make_unique<Edgedatabuilder<Networkproblem::Power::Transmissionline>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_type(), builder->get_factory()});
    }
    return data;
  }

}
