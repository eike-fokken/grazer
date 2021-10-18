#pragma once
#include "SimpleControlcomponent.hpp"

namespace Model {

  template <int N>
  class StaticControlcomponent : public SimpleControlcomponent {

  public:
    void set_initial_controls(
        Timedata timedata, Aux::InterpolatingVector &controller,
        nlohmann::json const &control_json) final;

    void set_lower_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> lower_bounds,
        nlohmann::json const &lower_bound_json) final;

    void set_upper_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> upper_bounds,
        nlohmann::json const &upper_bound_json) final;

  private:
    int needed_number_of_controls_per_time_point() const final;
  };

  extern template class StaticControlcomponent<1>;

} // namespace Model
