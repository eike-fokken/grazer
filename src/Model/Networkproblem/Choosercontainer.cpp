#include "Gaspowerconnection.hpp"
#include <Choosercontainer.hpp>
#include <memory>

#include <Pressureboundarynode.hpp>
#include <Flowboundarynode.hpp>
#include <Innode.hpp>
#include <Controlvalve.hpp>
#include <Compressorstation.hpp>
#include <PQnode.hpp>
#include <PVnode.hpp>
#include <Pipe.hpp>
#include <Shortpipe.hpp>
#include <Transmissionline.hpp>
#include <Vphinode.hpp>

namespace Model::Networkproblem::Netprob_Aux {

  Nodechooser::Nodechooser() {
    std::vector<std::unique_ptr<Nodedatabuilder_base>> buildervector;

    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::Vphinode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::PVnode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::PQnode>>());
    // buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Flowboundarynode>>());
    // buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Pressureboundarynode>>());
    // buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Innode>>());

    // Hier noch drüber nachdenken!
      
    // buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Source>>());
    // buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Sink>>());

    for (auto &builder : buildervector) {
      data.insert({builder->get_type(), builder->build_data()});
    }
  }

  std::map<std::string, Nodedata> Nodechooser::get_map()  {
    return data;
  }

  Edgechooser::Edgechooser() {
    std::vector<std::unique_ptr<Edgedatabuilder_base>> buildervector;

    buildervector.push_back(std::make_unique<Edgedatabuilder<Power::Transmissionline>>());
    // buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Pipe>>());
    // buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Shortpipe>>());
    // buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Controlvalve>>());
    // buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Compressorstation>>());
    // buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Gaspowerconnection>>());

    for (auto &builder : buildervector) {
      data.insert({builder->get_type(), builder->build_data()});
    }
  }

  std::map<std::string, Edgedata> Edgechooser::get_map() { return data; }

} // namespace Model::Networkproblem::Netprob_Aux
