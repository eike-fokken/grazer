#pragma once
#include <Eigen/Sparse>
#include <Subproblem.hpp> // This is really needed here!
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

  class Problem {

  public:
    /// The constructor needs to declare Delta_t
    ///
    Problem(
        nlohmann::json subproblem_data,
        std::filesystem::path const &_output_directory);

    ~Problem();

    void add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr);

    int set_indices();

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

    void save_values(double time, Eigen::Ref<Eigen::VectorXd> new_state);
    /// As we have unique pointers, we can only give back a pointer to our
    /// subproblems.
    std::vector<Subproblem *> get_subproblems() const;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state, nlohmann::json &initialjson);

    void print_to_files();

    std::filesystem::path const &get_output_directory() const;

  private:
    /// collection of sub-problems
    std::vector<std::unique_ptr<Subproblem>> subproblems;

    std::filesystem::path const output_directory;
  };

} // namespace Model
