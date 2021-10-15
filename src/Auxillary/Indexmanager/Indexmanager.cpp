#include "Indexmanager.hpp"
#include "Initialvalue.hpp"
#include "schema_validation.hpp"
#include <functional>

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

  template <int Values_per_step>
  void Timeless_Indexmanager<Values_per_step>::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled, int number_of_points,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema,
      std::function<Eigen::Matrix<double, Values_per_step, 1>(
          Eigen::Matrix<double, Values_per_step, 1>)>
          converter_function) {

    Aux::schema::validate_json(initial_json, initial_schema);
    Initialvalue<Values_per_step> initialvalues(initial_json);
    int current_index = get_startindex();
    for (int i = 0; i != number_of_points; ++i) {
      vector_to_be_filled.segment<Values_per_step>(current_index)
          = converter_function(initialvalues(i));
      current_index += number_of_points;
    }
  }

  template <int Values_per_step>
  void Timed_Indexmanager<Values_per_step>::set_initial_values(
      Model::Timedata timedata, Aux::Controller &vector_controller_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {
    Aux::schema::validate_json(initial_json, initial_schema);
  }

  template class Timeless_Indexmanager<1>;
  template class Timeless_Indexmanager<2>;
  template class Timeless_Indexmanager<4>;

} // namespace Aux
