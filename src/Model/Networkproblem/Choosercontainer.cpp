#include <Choosercontainer.hpp>
#include <memory>

#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"


namespace Model::Networkproblem::Netprob_Aux {

  Nodechooser::Nodechooser() {
    std::vector<std::unique_ptr<Nodedatabuilder_base>> buildervector;

    buildervector.push_back(
        std::make_unique<Nodedatabuilder<Power::Vphinode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::PVnode>>());
    buildervector.push_back(std::make_unique<Nodedatabuilder<Power::PQnode>>());

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

    for (auto &builder : buildervector) {
      data.insert({builder->get_type(), builder->build_data()});
    }
  }

  std::map<std::string, Edgedata> Edgechooser::get_map() { return data; }

} // namespace Model::Networkproblem::Netprob_Aux
