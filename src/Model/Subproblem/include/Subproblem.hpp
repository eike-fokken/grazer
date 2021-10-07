#pragma once
#include <Eigen/Sparse>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

namespace Aux {

  class Matrixhandler;
}

namespace Model {

  class Subproblem {

  public:
    /// \brief returns the type of an instance
    ///
    /// @returns the string that is the corresponding json key in the
    /// problem_data_file.json.
    virtual std::string get_type() const = 0;

    virtual ~Subproblem(){};

    // purely virtual functions:
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control)
        = 0;

    virtual void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evalutate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void
    json_save(double time, Eigen::Ref<Eigen::VectorXd const> const &state)
        = 0;

    virtual void add_results_to_json(nlohmann::json &new_output) = 0;

    /// Reserves indices from the state vector
    /// @param int next_free_index the first non-reserved index of the state
    /// vector.
    /// @returns int next_free_index the new first non-reserved index of the
    /// state vector.
    int set_state_indices(int const next_free_index);

    /// Reserves indices from the control vector
    /// @param int next_free_index the first non-reserved index of the control
    /// vector.
    /// @returns int next_free_index the new first non-reserved index of the
    /// control vector.
    int set_control_indices(int const next_free_index);

    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state, nlohmann::json initial_json)
        = 0;

    virtual void set_initial_controls_of_timestep(
        double time, Eigen::Ref<Eigen::VectorXd> controls,
        nlohmann::json const &control_json)
        = 0;

    int get_number_of_states() const;
    int get_start_state_index() const;
    int get_after_state_index() const;

    int get_number_of_controls() const;
    int get_start_control_index() const;
    int get_after_control_index() const;

  private:
    int start_state_index{0};
    int after_state_index{0};

    int start_control_index{0};
    int after_control_index{0};

    // std::vector<int> sporadic_state_indices;

    /// This function should reserve state indices, e.g. by setting start and
    /// end indices of subobjects. It can also do additional setup
    /// functionality, that is only applicable after indices have been reserved.
    /// @param The current smallest non-reserved index.
    /// @returns new smallest non-reserved index.
    virtual int reserve_state_indices(int const next_free_index) = 0;

    /// This function should reserve control indices, e.g. by setting start and
    /// end indices of subobjects. It can also do additional setup
    /// functionality, that is only applicable after indices have been reserved.
    /// @param The current smallest non-reserved index.
    /// @returns new smallest non-reserved index.
    virtual int reserve_control_indices(int const next_free_index) = 0;

    // have to think about the implementation of connecting problems.

    // int start_modeleq_index;
    // int end_modeleq_index;
    // std::vector<int> sporadic_modeleq_indices;
  };

} // namespace Model
