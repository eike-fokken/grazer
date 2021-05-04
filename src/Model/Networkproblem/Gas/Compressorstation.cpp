#include "Compressorstation.hpp"

#include "Get_base_component.hpp"
#include "make_schema.hpp"

namespace Model::Networkproblem::Gas {

  std::string Compressorstation::get_type() { return "Compressorstation"; }
  std::string Compressorstation::get_gas_type() const { return get_type(); }
  nlohmann::json Compressorstation::get_schema() {
    nlohmann::json schema = Shortpipe::get_schema();
    Aux::schema::add_required(
        schema, "control_values", Aux::schema::make_boundary_schema(1));
    return schema;
  }

  void Compressorstation::new_print_to_files(nlohmann::json &new_output) {
    std::string comp_type = Aux::component_class(*this);
    new_print_helper(new_output, comp_type, get_type());
  }

} // namespace Model::Networkproblem::Gas
