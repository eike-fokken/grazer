#include "Optimization_helpers.hpp"
#include "Controlhelpers.hpp"
#include "Exception.hpp"
#include "Networkproblem.hpp"
#include "Timeevolver.hpp"
#include <cassert>

namespace Optimization {

  void initialize_solvers(
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {
    assert(BT_vector.size() == solvers.size());
    for (std::size_t index = 0; index != BT_vector.size(); ++index) {
      solvers[index].analyzePattern(BT_vector[index]);
    }
  }

  void compute_multiplier(
      std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {

    // Use solver[index].factorize(BT_vector[index]) here and then
    // solver[index].solve( .... )
  }

  void compute_state_from_controls(
      Model::Timeevolver &timeevolver, Model::Networkproblem &problem,
      std::vector<Eigen::VectorXd> states,
      Eigen::Ref<Eigen::VectorXd const> const &controls) {}

  void initialize(
      Model::Timedata timedata, Model::Networkproblem &problem,
      Aux::Vector_interpolator &controller, nlohmann::json const &control_json,
      Eigen::Ref<Eigen::VectorXd> init_state,
      nlohmann::json const &initial_json,
      Eigen::Ref<Eigen::VectorXd> lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Eigen::Ref<Eigen::VectorXd> upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Eigen::Ref<Eigen::VectorXd> constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Eigen::Ref<Eigen::VectorXd> constraints_upper_bounds,
      nlohmann::json const &constraints_upper_bounds_json) {
    problem.set_initial_values(init_state, initial_json);
    problem.set_initial_controls(timedata, controller, control_json);
    problem.set_lower_bounds(timedata, lower_bounds, lower_bounds_json);
    problem.set_upper_bounds(timedata, upper_bounds, upper_bounds_json);
    problem.set_constraint_lower_bounds(
        timedata, constraints_lower_bounds, constraints_lower_bounds_json);
    problem.set_constraint_upper_bounds(
        timedata, constraints_upper_bounds, constraints_upper_bounds_json);
  }

} // namespace Optimization
