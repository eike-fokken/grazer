#pragma once
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem {
  class Controlcomponent {
  public:
    virtual ~Controlcomponent();

    /// \brief Returns number of controls needed by this component per time
    /// step.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 1.
    /// @returns number of control variables needed by this component
    virtual int get_number_of_controls() const = 0;

    /// \brief This function sets the indices #start_control_index and
    /// #after_control_index.
    ///
    /// @param next_free_index The first control index that is currently not
    /// claimed by another component.
    /// @returns The new lowest free index.
    int set_control_indices(int const next_free_index);

    /// \brief getter for #start_control_index
    int get_start_control_index() const;

    /// \brief getter for #after_control_index
    int get_after_control_index() const;

    /// \brief Fills the indices owned by this component with control values
    /// from a json.
    ///
    /// Relies on the exact format of the json.
    /// @param[out] new_state state vector, who shall contain the initial
    /// values.
    /// @param control_json Json object that contains the controls.
    virtual void set_controls_of_timestep(
        double time, Eigen::Ref<Eigen::VectorXd> controls,
        nlohmann::json const &control_json)
        = 0;

  private:
    /// \brief The first control index, this Controlcomponent "owns".
    int start_control_index{-1};

    /// \brief The first control index after #start_control_index, that is not
    /// "owned" by this Equationcomponent.
    int after_control_index{-1};
  };
} // namespace Model::Networkproblem
