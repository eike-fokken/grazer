#include "Compressorstation.hpp"

#include <make_schema.hpp>

namespace Model::Networkproblem::Gas {

  std::string Compressorstation::get_type() {return "Compressorstation";}
  nlohmann::json Compressorstation::get_schema() {
    nlohmann::json schema = Shortpipe::get_schema();
    Aux::schema::add_property(schema, "control_values", Aux::schema::make_boundary_schema(1));
    return schema;
  }
  
}
