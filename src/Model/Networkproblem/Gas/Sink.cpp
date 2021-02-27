#include "Sink.hpp"
#include "Flowboundarynode.hpp"

namespace Model::Networkproblem::Gas {

  template <typename Flowboundarynode>
  std::string Sink<Flowboundarynode>::get_type() {
    return "Sink";
  }

  template <typename Flowboundarynode>
  nlohmann::json Sink<Flowboundarynode>::revert_boundary_conditions(nlohmann::json const &data) {

    nlohmann::json reverted_boundary_json = data;

    for (auto &datum : reverted_boundary_json["data"]) {
      for (auto &value_json : datum["values"]) {
        double old_value = value_json.get<double>();
        value_json = -old_value;
      }
    }

    return reverted_boundary_json;
  }

  template <typename Flowboundarynode>
  Sink<Flowboundarynode>::Sink(nlohmann::json const &data)
      : Flowboundarynode(revert_boundary_conditions(data)) {}

 template class Sink<Pressurecouplingnode>;
 template class Sink<Bernoullicouplingnode>;

} // namespace Model::Networkproblem::Gas
