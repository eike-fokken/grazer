#include "Indexmanager.hpp"
#include "Control.hpp"
#include "Initialvalue.hpp"
#include "schema_validation.hpp"
#include <functional>
#include <string>

namespace Aux {

  Indexmanager::~Indexmanager() {}

  int Indexmanager::get_startindex() const {
    if (startindex < 0) {
      gthrow(
          {"startindex < 0. Probably get_startindex() was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return startindex;
  }
  int Indexmanager::get_afterindex() const {
    if (afterindex < 0) {
      gthrow(
          {"afterindex < 0. Probably get_afterindex() was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return afterindex;
  }
  int Indexmanager::get_number_of_indices() const {
    return get_afterindex() - get_startindex();
  }
  int Indexmanager::set_indices(
      int next_free_index, int number_of_needed_indices) {
    if (next_free_index < 0) {
      gthrow(
          {"Tried to set negative index: ", std::to_string(next_free_index)});
    }
    if (number_of_needed_indices <= 0) {
      gthrow(
          {"Tried to reserve zero or less number of indices: ",
           std::to_string(number_of_needed_indices)});
    }
    startindex = next_free_index;
    afterindex = next_free_index + number_of_needed_indices;
    return afterindex;
  }

  template <int Values_per_step>
  void Timeless_Indexmanager<Values_per_step>::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled, int number_of_points,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema,
      double Delta_x,
      std::function<Eigen::Matrix<double, Values_per_step, 1>(
          Eigen::Matrix<double, Values_per_step, 1>)>
          converter_function) {

    // first validate the json!
    Aux::schema::validate_json(initial_json, initial_schema);

    // Create a value map and fill the vector segment per segment.
    Initialvalue<Values_per_step> initialvalues(initial_json);
    int current_index = get_startindex();
    for (int i = 0; i != number_of_points; ++i) {
      vector_to_be_filled.segment<Values_per_step>(current_index)
          = converter_function(initialvalues(i * Delta_x));
      current_index += number_of_points;
    }
  }

  template <int Values_per_step>
  void Timed_Indexmanager<Values_per_step>::set_initial_values(
      Model::Timedata timedata, Aux::Controller &vector_controller_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {
    Aux::schema::validate_json(initial_json, initial_schema);

    auto starttime = timedata.get_starttime();
    auto delta_t = timedata.get_delta_t();
    auto no_timesteps = timedata.get_number_of_steps();

    // careful: no controls at t= starttime, because initial values are fixed!
    Control<Values_per_step> initialvalues(initial_json);
    for (int time_index = 1; time_index != no_timesteps; ++time_index) {
      vector_controller_to_be_filled.mut_timestep(time_index - 1)
          .segment(get_startindex(), Values_per_step)
          = initialvalues(starttime + time_index * delta_t);
    }
  }

  template class Timeless_Indexmanager<1>;
  template class Timeless_Indexmanager<2>;
  template class Timeless_Indexmanager<4>;

  template class Timed_Indexmanager<1>;
  template class Timed_Indexmanager<2>;

} // namespace Aux
