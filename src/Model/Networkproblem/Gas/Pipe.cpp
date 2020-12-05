#include "Implicitboxscheme.hpp"
#include "Isothermaleulerequation.hpp"
#include <Pipe.hpp>
#include <Mathfunctions.hpp>

namespace Model::Networkproblem::Gas {

  Pipe::Pipe(std::string _id, Network::Node *start_node,
             Network::Node *end_node, nlohmann::ordered_json topology_json)
      : Gasedge(_id, start_node, end_node),
        length(topology_json["length"]["value"].get<double>() * 1e3),
        diameter(topology_json["diameter"]["value"].get<double>() * 1e-3),
        roughness(topology_json["roughness"]["value"].get<double>()),
        bl(Balancelaw::Isothermaleulerequation(0.25* Aux::Pi * diameter * diameter , diameter, roughness)),
        scheme(Model::Scheme::Implicitboxscheme(bl)) { }

}
