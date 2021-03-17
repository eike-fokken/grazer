#include "Gas_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"

namespace Model::Componentfactory {

  std::map<std::string, Nodefactory> Gas_factory::get_nodetypemap_impl(){

    std::map<std::string, Nodefactory> data;
        std::vector<std::unique_ptr<AbstractNodeType>> buildervector;

    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_type(), builder->get_factory()});
    }

    return data;
  }
  std::map<std::string, Edgefactory> Gas_factory::get_edgetypemap_impl(){
    std::map<std::string, Edgefactory> data;
    std::vector<std::unique_ptr<Edgedatabuilder_base>> buildervector;

    buildervector.push_back(std::make_unique<Edgedatabuilder<Networkproblem::Gas::Pipe>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Networkproblem::Gas::Shortpipe>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Networkproblem::Gas::Controlvalve>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Networkproblem::Gas::Compressorstation>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Networkproblem::Gas::Gaspowerconnection>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_type(), builder->get_factory()});
    }
    return data;
  }

}
