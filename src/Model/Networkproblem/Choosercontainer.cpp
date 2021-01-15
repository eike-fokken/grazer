#include <Choosercontainer.hpp>
#include <memory>

#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Vphinode.hpp"


namespace Model::Networkproblem::Netprob_Aux {

  Nodechooser::Nodechooser() {
    std::vector <std::unique_ptr<Nodetypedatabuilder_base> >  buildervector;

    buildervector.push_back(std::make_unique<Nodetypedatabuilder<Power::Vphinode>>());
    buildervector.push_back(std::make_unique<Nodetypedatabuilder<Power::PVnode>>());
    buildervector.push_back(std::make_unique<Nodetypedatabuilder<Power::PQnode>>());


    for (auto & builder :buildervector) {
      data.insert({builder->get_type(),builder->build_data()});
    }
  }

  std::map<std::string, Nodetypedata> Nodechooser::get_map()  {
    return data;
  }

} // namespace Model::Networkproblem::Netprob_Aux
