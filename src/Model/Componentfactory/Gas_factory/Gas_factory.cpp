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

  void add_gas_components(Componentfactory &factory) {
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Pipe>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Shortpipe>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Controlvalve>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Compressorstation>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Gaspowerconnection>>());
  }

  std::map<std::string, Nodefactory> Gas_factory::get_nodetypemap_impl(){

    std::map<std::string, Nodefactory> data;
        std::vector<std::unique_ptr<AbstractNodeType>> buildervector;

    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    buildervector.push_back(std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_name(), builder->get_factory()});
    }

    return data;
  }
  std::map<std::string, Edgefactory> Gas_factory::get_edgetypemap_impl(){
    std::map<std::string, Edgefactory> data;
    std::vector<std::unique_ptr<AbstractEdgeType>> buildervector;

    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Pipe>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Shortpipe>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Controlvalve>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Compressorstation>>());
    buildervector.push_back(std::make_unique<EdgeType<Networkproblem::Gas::Gaspowerconnection>>());

    for (auto const &builder : buildervector) {
      data.insert({builder->get_name(), builder->get_factory()});
    }
    return data;
  }

}
