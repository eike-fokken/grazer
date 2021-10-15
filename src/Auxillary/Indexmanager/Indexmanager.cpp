#include "Indexmanager.hpp"
#include "schema_validation.hpp"

namespace Aux {

  Indexmanager::~Indexmanager() {}

  int Indexmanager::get_startindex() const { return startindex; }
  int Indexmanager::get_afterindex() const { return afterindex; }
  int Indexmanager::get_number_of_indices() const {
    return afterindex - startindex;
  }
  int Indexmanager::set_indices(
      int next_free_index, int number_of_needed_indices) {
    startindex = next_free_index;
    afterindex = next_free_index + number_of_needed_indices;
    return afterindex;
  }

  void Timeless_Indexmanager::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {
    Aux::schema::validate_json(initial_json, initial_schema);
  }

  void Timed_Indexmanager::set_initial_values(
      Model::Timedata timedata, Aux::Controller &vector_controller_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {
    Aux::schema::validate_json(initial_json, initial_schema);
  }

} // namespace Aux
