#include "SimpleControlcomponent.hpp"
#include "InterpolatingVector.hpp"
namespace Model {

  void set_simple_control_values(
      Controlcomponent const *controlcomponent,
      Aux::InterpolatingVector_Base &full_control_vector,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {

    auto &timepoints = full_control_vector.get_interpolation_points();

    // careful: no controls at t= starttime, because initial values are fixed!
    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);

    auto number_of_indices_per_step = initialvalues.get_inner_length();

    Eigen::Index index = 0;
    for (auto timepoint : timepoints) {
      full_control_vector.mut_timestep(index).segment(
          controlcomponent->get_control_startindex(),
          number_of_indices_per_step)
          = initialvalues(timepoint);
      ++index;
    }
  }

  Eigen::Index
  SimpleControlcomponent::set_control_indices(Eigen::Index next_free_index) {
    control_startindex = next_free_index;
    auto number_of_controls = needed_number_of_controls_per_time_point();
    control_afterindex = next_free_index + number_of_controls;

    return control_afterindex;
  }

} // namespace Model
