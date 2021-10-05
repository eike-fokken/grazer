#pragma once

#include <Eigen/Sparse>
// #include <Subproblem.hpp> // This is really needed here!
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

namespace Aux {

  class Matrixhandler;
}

/*! \namespace Model
 *  This namespace holds all data for setting up model equations and for taking
 * derivatives thereof.
 */
namespace Model {
  class Subproblem;

  class Problem {

  public:
    Problem(
        nlohmann::json subproblem_data,
        std::filesystem::path const &_output_file);

    ~Problem();

    void add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr);

    int set_state_indices();

    int set_control_indices();

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control);

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const;

    void json_save(double time, Eigen::Ref<Eigen::VectorXd const> const &state);

    /// As we have unique pointers, we can only give back a pointer to our
    /// subproblems.
    std::vector<Subproblem *> get_subproblems() const;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state, nlohmann::json &initialjson);

    void add_results_to_json();

    std::filesystem::path const &get_output_file() const;

  private:
    /// collection of sub-problems
    std::vector<std::unique_ptr<Subproblem>> subproblems;

    std::filesystem::path const output_file;
    nlohmann::json json_output;
  };

} // namespace Model
