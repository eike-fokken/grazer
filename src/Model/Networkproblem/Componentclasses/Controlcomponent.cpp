#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include <nlohmann/json.hpp>

namespace Model {
  void Controlcomponent::prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) {}

  Eigen::Index Controlcomponent::get_number_of_controls_per_timepoint() const {
    return get_control_afterindex() - get_control_startindex();
  }

  Eigen::Index Controlcomponent::get_control_startindex() const {
    if (control_startindex < 0) {
      gthrow(
          {"control_startindex < 0. Probably ", __func__, " was called ",
           "before calling set_indices().\n This is forbidden."});
    }
    return control_startindex;
  }
  Eigen::Index Controlcomponent::get_control_afterindex() const {
    if (control_afterindex < 0) {
      gthrow(
          {"control_afterindex < 0. Probably ", __func__,
           " was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return control_afterindex;
  }

  void Controlcomponent::save_controls_to_json(
      Aux::InterpolatingVector_Base const &controls, nlohmann::json &json) {
    auto this_class = componentclass();
    auto this_type = componenttype();
    if (not json.contains(this_class)) {
      json[this_class] = nlohmann::json::object();
    }
    if (not json[this_class].contains(this_type)) {
      json[this_class][this_type] = nlohmann::json::array();
    }

    nlohmann::json output_json;
    output_json["id"] = id();
    output_json["data"] = nlohmann::json::array();

    for (Eigen::Index index = 0; index != controls.size(); ++index) {
      nlohmann::json this_timestep;
      this_timestep["time"] = controls.interpolation_point_at_index(index);
      std::vector<double> controlvalues(
          static_cast<size_t>(get_number_of_controls_per_timepoint()));

      auto startindex = get_control_startindex();
      for (size_t value_index = 0; value_index != controlvalues.size();
           ++value_index) {
        auto sindex = static_cast<Eigen::Index>(value_index);
        controlvalues[value_index]
            = controls.vector_at_index(index)[startindex + sindex];
      }
      this_timestep["values"] = controlvalues;
      output_json["data"].push_back(std::move(this_timestep));
    }
    json[this_class][this_type].push_back(std::move(output_json));
  }

} // namespace Model
