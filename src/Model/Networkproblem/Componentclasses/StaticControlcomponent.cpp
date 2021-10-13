#include "StaticControlcomponent.hpp"
#include "Control.hpp"
#include "Controlhelpers.hpp"
namespace Model {

  template <int N>
  void StaticControlcomponent<N>::set_initial_controls(
      Timedata timedata, Aux::Controller &controller,
      nlohmann::json const &control_json) {

    /*
     *Man muss mit dem Controller arbeiten, sonst geht das nicht!
     */
    Control<N> controlmap(control_json);
    auto starttime = timedata.get_starttime();
    auto endtime = timedata.get_endtime();
    auto number_of_timesteps = timedata.get_number_of_steps();
    auto number_of_controls_per_timepoint
        = needed_number_of_controls_per_time_point();

    double new_time = starttime;
    for (int timeindex = 0; timeindex != number_of_timesteps; ++timeindex) {
      assert(false);
    }
  }

  template <int N>
  void StaticControlcomponent<N>::set_lower_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> lower_bounds,
      nlohmann::json const &lower_bound_json) {}

  template <int N>
  void StaticControlcomponent<N>::set_upper_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> upper_bounds,
      nlohmann::json const &upper_bound_json) {}

  template <int N>
  int StaticControlcomponent<N>::needed_number_of_controls_per_time_point()
      const {
    return N;
  }

} // namespace Model
