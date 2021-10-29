#pragma once
#include "Controlcomponent.hpp"
namespace Model {

  void set_simple_control_values(
      Controlcomponent const *controlcomponent,
      Aux::InterpolatingVector &full_control_vector,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema);

  class SimpleControlcomponent : public Controlcomponent {
  public:
    Eigen::Index set_control_indices(Eigen::Index next_free_index) final;

  private:
    /**
     * @brief Returns number of control variables needed by this component.
     *
     * Often this will be implemented by a function returning a literal
     * int like 2.
     *
     * @returns number of control variables needed by this component
     */
    virtual Eigen::Index needed_number_of_controls_per_time_point() const = 0;
  };
} // namespace Model
