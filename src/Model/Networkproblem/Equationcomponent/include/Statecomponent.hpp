#pragma once
#include "Equationcomponent.hpp"

namespace Model::Networkproblem {
  class Statecomponent : public Equationcomponent {
  public:
    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component
    virtual int get_number_of_states() const = 0;

    /// \brief This function sets the indices #start_state_index and
    /// #after_state_index AND calls setup()
    ///
    /// @param next_free_index The first index that is currently not claimed by
    /// another component.
    /// @returns The new lowest free index.
    int set_indices(int const next_free_index);

    /// \brief getter for #start_state_index
    int get_start_state_index() const;

    /// \brief getter for #after_state_index
    int get_after_state_index() const;

    /// \brief Prints values contained in #values_ptr into a file in
    /// output_directory.
    virtual void print_to_files(std::filesystem::path const &output_directory)
        = 0;

    virtual void new_print_to_files(nlohmann::json &){};
    /// \brief Saves values at the owned indices of this component into
    /// #values_ptr.
    ///
    /// @param time time of the timestep to be saved.
    /// @param state State of all variables. Only the values in the owned
    /// indices are saved.
    virtual void
    save_values(double time, Eigen::Ref<Eigen::VectorXd const> state)
        = 0;

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

    /// \brief helper function for save_values() that deals with the data
    /// structure of #values_ptr
    ///
    /// @param t time to be inserted
    /// @param valuemap map of values to be inserted. key is the x position on
    /// the component. x==0 for components without spatial dimension.
    void
    push_to_values(double t, std::vector<std::map<double, double>> valuemap);

    /// \brief getter for the vector of times saved in #values_ptr
    std::vector<double> const &get_times() const;

    /// \brief getter for the vector of valuemaps saved in #values_ptr
    std::vector<std::vector<std::map<double, double>>> const &
    get_values() const;

    nlohmann::json &get_output_json_ref();

  private:
    /// \brief a container class to group the times and values together for
    /// later access for printout. As this may change we don't expose it
    /// publicly.
    struct Valuecontainer {
      std::vector<double> times;
      std::vector<std::vector<std::map<double, double>>> values;
    };

    /// \brief contains the computed values of states for use in the
    /// print_to_files() method.
    std::unique_ptr<Valuecontainer> values_ptr{
        std::make_unique<Valuecontainer>()};

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
