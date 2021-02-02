#include "Exception.hpp"
#include <Eigen/Dense>
#include <Matrixhandler.hpp>
#include <Problem.hpp>
#include <Subproblem.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <Subproblemchooser.hpp>
#include <Aux_json.hpp>

namespace Model {

  Problem::Problem(nlohmann::json problem_data,std::filesystem::path const &_output_directory)
      : output_directory(_output_directory) {
    auto subproblem_map = problem_data["subproblems"].get<std::map<std::string, nlohmann::json>>();
    for(auto & [subproblem_type,subproblem_json] : subproblem_map)
      {
        subproblem_json["GRAZER_file_directory"] = problem_data["GRAZER_file_directory"];
        std::unique_ptr<Subproblem> subproblem_ptr =
            Subproblemchooser::build_subproblem(subproblem_type,
                                                subproblem_json);
        add_subproblem(std::move(subproblem_ptr));
    }
  }


  Problem::~Problem() {print_to_files();}


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
                                   nlohmann::json initial_json) {

    for (auto &subproblem : subproblems) {
      auto type = subproblem->get_type();

      auto initial_json_iterator = initial_json["subproblems"].find(type);
      if (initial_json_iterator == initial_json["subproblems"].end()){
        gthrow({"Subproblem ", type, " has no initial values in the json files!"});
      }

      auto & subproblem_initial_json = *initial_json_iterator;
      std::string initial_key =
        "initial_json";
      aux_json::replace_entry_with_json_from_file(subproblem_initial_json, initial_key);
              subproblem->set_initial_values(new_state, subproblem_initial_json);
    }
  }


  std::filesystem::path const &Problem::get_output_directory() const {
    return output_directory;
  }

} // namespace Model
