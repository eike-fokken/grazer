#include "Indexmanager.hpp"
#include "Control.hpp"
#include "Initialvalue.hpp"
#include "InterpolatingVector.hpp"
#include "schema_validation.hpp"
#include <functional>
#include <string>

namespace Aux {

  Eigen::VectorXd
  identity_converter(Eigen::Ref<Eigen::VectorXd const> const &x) {
    return x;
  }

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

  void Timeless_Indexmanager::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled, int number_of_points,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema,
      double Delta_x,
      std::function<Eigen::VectorXd(Eigen::Ref<Eigen::VectorXd const> const &)>
          converter_function) {

    Aux::schema::validate_json(initial_json, initial_schema);

    // Create an InterpolatingVector it segment per segment.
    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);
    auto number_of_indices_per_step = initialvalues.get_inner_length();

    int current_index = get_startindex();
    if (number_of_indices_per_step * number_of_points
        > get_number_of_indices()) {
      gthrow(
          {"You try to set more initial values than are reserved for "
           "component\n",
           ">>>", initial_json["id"], "<<<\n"});
    }
    for (int i = 0; i != number_of_points; ++i) {
      vector_to_be_filled.segment(current_index, number_of_indices_per_step)
          = converter_function(initialvalues(i * Delta_x));
      current_index += number_of_points;
    }
  }

  void Timed_Indexmanager::set_initial_values(
      Model::Timedata timedata,
      Aux::InterpolatingVector &vector_controller_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {

    auto starttime = timedata.get_starttime();
    auto delta_t = timedata.get_delta_t();
    auto no_timepoints = timedata.get_number_of_time_points();

    // careful: no controls at t= starttime, because initial values are fixed!
    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);

    auto number_of_indices_per_step = initialvalues.get_inner_length();

    for (int time_index = 0; time_index != no_timepoints; ++time_index) {
      vector_controller_to_be_filled.mut_timestep(time_index)
          .segment(get_startindex(), number_of_indices_per_step)
          = initialvalues(starttime + time_index * delta_t);
    }
  }

} // namespace Aux
