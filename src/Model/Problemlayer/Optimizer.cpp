#include "Optimizer.hpp"
#include "Timeevolver.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

namespace Model {

  nlohmann::json Optimizer::get_schema() {
    nlohmann::json schema;
    schema["type"] = "object";
    return schema;
  }

  Optimizer::Optimizer(
      nlohmann::json const &optimization_json,
      nlohmann::json const &time_evolution_json) :
      timeevolver(Timeevolver::make_instance(time_evolution_json)){};

  Optimizer Optimizer::make_instance(
      nlohmann::json const &optimization_json,
      nlohmann::json const &time_evolution_json) {
    Aux::schema::validate_json(optimization_json, get_schema());
    return Optimizer(optimization_json, time_evolution_json);
  }
} // namespace Model
