#include "Indexmanager.hpp"
#include "schema_validation.hpp"

namespace Aux {
  int Indexmanager::get_startindex() const { return startindex; }
  int Indexmanager::get_afterindex() const { return afterindex; }

  int Indexmanager::get_number_of_indices() const {
    return afterindex - startindex;
  }
  int Indexmanager::set_state_indices(
      int next_free_index, int number_of_needed_indices) {
    startindex = next_free_index;
    afterindex = next_free_index + number_of_needed_indices;
    return afterindex;
  }

  void Indexmanager::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> vector_to_fill,
      nlohmann::json const &initial_values_json, nlohmann::json const &schema) {

    assert(
        false); // Difficult to disentangle time-dependet initial values
                // (controls, inequlities) and one-time initial values (states)
    Aux::schema::validate_json(initial_values_json, schema);
    for (int index = startindex; index != afterindex; ++index) {
      vector_to_fill[index] = initial_values_json[]
    }
  }

} // namespace Aux
