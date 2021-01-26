#include <Eigen/Dense>
#include <Matrixhandler.hpp>
#include <Problem.hpp>
#include <Subproblem.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <Subproblemchooser.hpp>

namespace Model {

  Problem::Problem(nlohmann::json subproblem_data,std::filesystem::path const &_output_directory)
      : output_directory(_output_directory) {
    auto subproblem_map = subproblem_data.get<std::map<std::string, nlohmann::json>>();
    for(auto const & [subproblem_type,subproblem_json] : subproblem_map)
      {
        std::unique_ptr<Subproblem> subproblem_ptr = Subproblemchooser::build_subproblem(subproblem_type, subproblem_json);
        add_subproblem(std::move(subproblem_ptr));
      }
  }

  void Problem::add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr) {
    subproblems.push_back(std::move(subproblem_ptr));
  }

  int Problem::set_indices() {
    int next_free_index(0);
    for (auto &subproblem : subproblems) {
      next_free_index = subproblem->set_indices(next_free_index);
    }
    return next_free_index;
  }

  void Problem::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                         double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                         Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (auto &subproblem : subproblems) {
      subproblem->evaluate(rootvalues, last_time, new_time, last_state,
                           new_state);
    }
  }

  void Problem::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                          double last_time, double new_time,
                                          Eigen::Ref<Eigen::VectorXd const> const &last_state,
                                          Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (auto &subproblem : subproblems) {
      subproblem->evaluate_state_derivative(jacobianhandler, last_time,
                                            new_time, last_state, new_state);
    }
  }

  void Problem::save_values(double time, Eigen::Ref<Eigen::VectorXd>new_state) {
    for (auto &subproblem : subproblems) {
      subproblem->save_values(time, new_state);
    }
  }

  std::vector<Subproblem *> Problem::get_subproblems() const {
    std::vector<Subproblem *> pointer_vector;
    for (auto &subproblem : subproblems) {
      pointer_vector.push_back(subproblem.get());
    }
    return pointer_vector;
  }

  void Problem::print_to_files() {
    for (auto &subproblem : subproblems) {
      subproblem->print_to_files(output_directory);
    }
  }

  void Problem::display() const {
    for (auto &subproblem : subproblems) {
      subproblem->display();
    }
  }

  void Problem::set_initial_values(Eigen::Ref<Eigen::VectorXd> new_state,
                                   nlohmann::ordered_json initialjson) {
    for (auto &subproblem : subproblems) {
      subproblem->set_initial_values(new_state, initialjson);
    }
  }

  std::filesystem::path const &Problem::get_output_directory() const {
    return output_directory;
  }

} // namespace Model
