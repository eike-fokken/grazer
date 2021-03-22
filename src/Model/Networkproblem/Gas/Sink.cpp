#include "Sink.hpp"

namespace Model::Networkproblem::Gas {

  std::string Sink::get_type() {return "Sink";}

  nlohmann::json Sink::revert_boundary_conditions(nlohmann::json const &data){

    nlohmann::json reverted_boundary_json = data;

    for (auto &datum : reverted_boundary_json["data"]) {
      for (auto &value_json : datum["values"]) {
        double old_value = value_json.get<double>();
        value_json = -old_value;
      }
    }

    return reverted_boundary_json;
  }

  Sink::Sink(nlohmann::json const & data):Flowboundarynode(revert_boundary_conditions(data)){}

} // namespace Model::Networkproblem::Gas
