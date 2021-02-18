#include "Componentfactory.hpp"
#include "Componentchooser.hpp"
#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Flowboundarynode.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Pipe.hpp"
#include "Source.hpp"
#include "Sink.hpp"
#include "Pressureboundarynode.hpp"
#include "Shortpipe.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"
#include <memory>

namespace Model::Networkproblem::Componentfactory {

  Nodechooser::Nodechooser() {
    std::vector<std::unique_ptr<Nodedatabuilder_base>> buildervector;

    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::Vphinode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::PVnode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::PQnode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Source>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Sink>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Gas::Innode>>());

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
    buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Pipe>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Shortpipe>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Controlvalve>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Compressorstation>>());
    buildervector.push_back(std::make_unique<Edgedatabuilder<Gas::Gaspowerconnection>>());

    for (auto &builder : buildervector) {
      data.insert({builder->get_type(), builder->build_data()});
    }
  }

  std::map<std::string, Edgedata> Edgechooser::get_map() { return data; }

} // namespace Model::Networkproblem::Netprob_Aux
