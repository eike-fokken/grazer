#pragma once
#include <Eigen/Sparse>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {

  /// \brief A base class for network components that supply model equations to
  /// be solved.
  ///
  /// An equation component owns a number of indices, namely between
  /// #start_state_index (inclusive) and #after_state_index (exclusive), which
  /// will be filled with results of the model equations of the component.
  /// It is implicitely taken for granted that the set of owned indices of all
  /// objects starts at 0 and is consecutive.
  class Equationcomponent {

  public:
    virtual ~Equationcomponent(){};

    /// \brief evaluates the model equations into rootvalues.
    ///
    /// @param[out] rootvalues Results of the model equations, when evaluated on
    /// the other parameters.
    /// @param last_time time point of the last time step. Usually important for
    /// PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const = 0;

    /// \brief Carries out steps that need to be taken before the Newton method
    /// for a time step can start.
    ///
    /// For most components does nothing.
    /// @param last_time time point of the last time step. Usually important for
    /// PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state);

    /// \brief derivative of Equationcomponent::evaluate.
    ///
    /// evaluates the derivative of Equationcomponent::evaluate and hands
    /// the result to jacobianhandler, which will fill the Jacobi matrix.
    /// @param jacobianhandler A helper object, that fills a sparse matrix
    /// in an efficient way.
    /// @param last_time time point of the last time step. Usually important
    /// for PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const = 0;

    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component

    virtual int get_number_of_states() const = 0;

    /// \brief Utility for setup of things that cannot be done during
    /// construction.
    ///
    /// is called during set_indices.
    /// Usually does nothing, but for example gas nodes
    /// claim indices from their attached gas edges.
    virtual void setup();

    void setup_helper(std::string const &id);

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

    /// \brief Returns true, if the concrete equation components wants to print
    /// to output files.
    ///
    /// Defaults to true, but specific components can overload this function.
    /// @returns bool, is true, if the component wants to write to files.
    static bool needs_output_file();

  protected:
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
      nlohmann::json component_output;
      std::vector<double> times;
      std::vector<std::vector<std::map<double, double>>> values;
    };

    /// \brief contains the computed values of states for use in the
    /// print_to_files() method.
    std::unique_ptr<Valuecontainer> values_ptr{
        std::make_unique<Valuecontainer>()};

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
