#pragma once
#include "Statecomponent.hpp"
namespace Model {

  Eigen::VectorXd
  identity_converter(Eigen::Ref<Eigen::VectorXd const> const &x);

  void set_simple_initial_values(
      Statecomponent const *component,
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema,
      Eigen::Index number_of_points = 1, double Delta_x = 1.0,
      std::function<Eigen::VectorXd(Eigen::Ref<Eigen::VectorXd const> const &)>
          converter_function
      = identity_converter);

  class SimpleStatecomponent : public Statecomponent {
  public:
    Eigen::Index set_state_indices(Eigen::Index next_free_index) final;

  private:
    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component
    virtual Eigen::Index needed_number_of_states() const = 0;
  };
} // namespace Model
