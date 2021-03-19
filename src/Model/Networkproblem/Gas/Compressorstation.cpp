#include "Compressorstation.hpp"

#include <make_schema.hpp>

namespace Model::Networkproblem::Gas {

  std::string Compressorstation::get_type() {return "Compressorstation";}
  nlohmann::json Compressorstation::get_schema() {
    nlohmann::json schema = Shortpipe::get_schema();
    Aux::schema::add_property(schema, "control_values", Aux::schema::make_boundary_schema(1));

    Aux::schema::add_required(schema, "gas2power_q_coeff", Aux::schema::type::number());
    Aux::schema::add_required(schema, "power2gas_q_coeff", Aux::schema::type::number());

    return schema;
  }
  
}
