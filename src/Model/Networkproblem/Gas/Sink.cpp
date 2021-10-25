#include "Sink.hpp"
#include "Exception.hpp"

namespace Model::Gas {

  nlohmann::json revert_boundary_conditions(nlohmann::json const &data) {

    nlohmann::json reverted_boundary_json = data;
    if (not reverted_boundary_json.contains("boundary_values")) {
      gthrow(
          {"The sink json ", data["id"],
           " did not contain boundary values.  Something went wrong.\n",
           "Its json was: ", data.dump(1, '\t')});
    }
    for (auto &datum : reverted_boundary_json["boundary_values"]["data"]) {
      for (auto &value_json : datum["values"]) {
        double old_value = value_json.get<double>();
        value_json = -old_value;
      }
    }

    return reverted_boundary_json;
  }

  Sink::Sink(nlohmann::json const &data) :
      Flowboundarynode(revert_boundary_conditions(data)) {}

} // namespace Model::Gas
