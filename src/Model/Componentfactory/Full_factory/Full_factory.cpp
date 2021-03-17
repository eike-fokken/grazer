#include "Full_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  std::map<std::string, Nodefactory> Full_factory::get_nodetypemap_impl(){

    std::map<std::string, Nodefactory> data;
        std::vector<std::unique_ptr<AbstractNodeType>> buildervector;

    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Power::PQnode>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_type(), builder->get_factory()});
    }

    return data;
  }
  std::map<std::string, Edgefactory> Full_factory::get_edgetypemap_impl(){
    std::map<std::string, Edgefactory> data;
    std::vector<std::unique_ptr<AbstractEdgeType>> buildervector;

    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Power::Transmissionline>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Pipe>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Shortpipe>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Controlvalve>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Compressorstation>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Gaspowerconnection>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_type(), builder->get_factory()});
    }
    return data;
  }

}
