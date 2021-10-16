#include "StaticControlcomponent.hpp"
#include "Controlhelpers.hpp"
namespace Model {

  template <int N>
  void StaticControlcomponent<N>::set_initial_controls(
      Timedata timedata, Aux::Vector_interpolator &controller,
      nlohmann::json const &control_json) {

    assert(false); // Not sure this function works as intended!
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
  template class StaticControlcomponent<1>;
} // namespace Model
