#pragma once
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
// #include "Equationcomponent.hpp"

namespace Model::Networkproblem {
  class Statecomponent {
  public:
    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component
    virtual int get_number_of_states() const = 0;

    static nlohmann::json get_initial_schema();

    /// \brief This function sets the indices #start_state_index and
    /// #after_state_index.
    ///
    /// @param next_free_index The first state index that is currently not
    /// claimed by another component.
    /// @returns The new lowest free index.
    int set_state_indices(int const next_free_index);

    /// \brief getter for #start_state_index
    int get_start_state_index() const;

    /// \brief getter for #after_state_index
    int get_after_state_index() const;

    virtual void add_results_to_json(nlohmann::json &) = 0;

    /// \brief Stores computed values in an output json.
    virtual void json_save(double time, Eigen::Ref<Eigen::VectorXd const> state)
        = 0;

    /// \brief Fills the indices owned by this component with initial values
    /// from a json.
    ///
    /// Relies on the exact format of the json. \todo { Document the intial json
    /// format. }
    /// @param[out] new_state state vector, who shall contain the initial
    /// values.
    /// @param initial_json Json object that contains the initial values.
    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json)
        = 0;

  protected:
    void setup_output_json_helper(std::string const &id);

    nlohmann::json &get_output_json_ref();

  private:
    /// \brief holds the computed values of this component.
    ///
    /// Contains exactly two keys: "time" and "data"
    /// Time holds a number representing the time in secondes.
    /// Data holds an array of jsons, whose layout depends on the component in
    /// question.
    nlohmann::json component_output;
    /// \brief The first index, this Equationcomponent "owns".
    ///
    /// Most equation components write only to their own indices between
    /// #start_state_index (inclusive) and #after_state_index (exclusive).
    /// There are exceptions though, e.g. instances of \ref
    /// Model::Networkproblem::Gas::Gasnode "Gasnode".
    int start_state_index{-1};

    /// \brief The first index after #start_state_index, that is not "owned" by
    /// this Equationcomponent.
    ///
    /// Most equation components write only to their own indices between
    /// #start_state_index (inclusive) and #after_state_index (exclusive).
    /// There are exceptions though, e.g. instances of
    /// \ref Model::Networkproblem::Gas::Gasnode "Gasnode".
    int after_state_index{-1};
  };
} // namespace Model::Networkproblem
