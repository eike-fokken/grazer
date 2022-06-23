#include "Sink.hpp"
#include "Exception.hpp"

namespace Model::Gas {

  std::string Sink::get_type() { return "Sink"; }

  Sink::Sink(nlohmann::json const &data) :
      Flowboundarynode(revert_boundary_conditions(data)) {}

} // namespace Model::Gas
