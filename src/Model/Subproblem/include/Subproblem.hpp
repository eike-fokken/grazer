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
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const = 0;

    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state)
        = 0;

    virtual void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const = 0;

    virtual void json_save(double time, Eigen::Ref<Eigen::VectorXd const> state)
        = 0;

    virtual void print_to_files(nlohmann::json &new_output) = 0;

    // Reserves indices from the state vector
    // @param int next_free_index the first non-reserved index of the state
    // vector.
    // @returns int next_free_index the new first non-reserved index of the
    // state vector.
    int set_indices(int const next_free_index);

    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state, nlohmann::json initial_json)
        = 0;

    int get_number_of_states() const;
    int get_start_state_index() const;
    int get_after_state_index() const;

    // void set_sporadic_state_indices(std::vector<int> indices);

  private:
    int start_state_index{0};
    int after_state_index{0};
    // std::vector<int> sporadic_state_indices;

    /// This function should reserve indices, e.g. by setting start and end
    /// indices of subobjects
    /// @param The current smallest non-reserved index.
    /// @returns new smallest non-reserved index.
    virtual int reserve_indices(int const next_free_index) = 0;

    // have to think about the implementation of connecting problems.

    // int start_modeleq_index;
    // int end_modeleq_index;
    // std::vector<int> sporadic_modeleq_indices;
  };

} // namespace Model
