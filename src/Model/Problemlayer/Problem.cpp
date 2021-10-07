#include "Problem.hpp"
#include "Aux_json.hpp"
#include "Exception.hpp"
#include "Matrixhandler.hpp"
#include "Subproblem.hpp"
#include "Subproblemchooser.hpp"
#include <Eigen/Dense>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

namespace Model {

  Problem::Problem(
      nlohmann::json problem_data, std::filesystem::path const &_output_file) :
      output_file(_output_file) {
    auto subproblem_map = problem_data["subproblems"]
                              .get<std::map<std::string, nlohmann::json>>();
    for (auto &[subproblem_type, subproblem_json] : subproblem_map) {
      subproblem_json["GRAZER_file_directory"]
          = problem_data["GRAZER_file_directory"];
      std::unique_ptr<Subproblem> subproblem_ptr
          = build_subproblem(subproblem_type, subproblem_json);
      add_subproblem(std::move(subproblem_ptr));
    }
  }

  Problem::~Problem() {
    try {
      // add_results_to_json();
      add_results_to_json();
    } catch (std::exception &e) {
      std::cout << "Printing to files failed with error message:"
                << "\n###############################################\n"
                << e.what()
                << "\n###############################################\n\n"
                << std::flush;
    }
  }

  void Problem::add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr) {
    subproblems.push_back(std::move(subproblem_ptr));
  }

  int Problem::set_state_indices() {
    int next_free_index(0);
    for (auto &subproblem : subproblems) {
      next_free_index = subproblem->set_state_indices(next_free_index);
    }
    return next_free_index;
  }

  int Problem::set_control_indices() {
    int next_free_index(0);
    for (auto &subproblem : subproblems) {
      next_free_index = subproblem->set_control_indices(next_free_index);
    }
    return next_free_index;
  }

  void Problem::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto &subproblem : subproblems) {
      subproblem->evaluate(
          rootvalues, last_time, new_time, last_state, new_state, control);
    }
  }

  void Problem::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) {
    for (auto &subproblem : subproblems) {
      subproblem->prepare_timestep(
          last_time, new_time, last_state, new_state, control);
    }
  }

  void Problem::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto &subproblem : subproblems) {
      subproblem->d_evalutate_d_new_state(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Problem::d_evalutate_d_last_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto &subproblem : subproblems) {
      subproblem->d_evalutate_d_last_state(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Problem::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    for (auto &subproblem : subproblems) { subproblem->json_save(time, state); }
  }

  std::vector<Subproblem *> Problem::get_subproblems() const {
    std::vector<Subproblem *> pointer_vector;
    for (auto &subproblem : subproblems) {
      pointer_vector.push_back(subproblem.get());
    }
    return pointer_vector;
  }

  void Problem::add_results_to_json() {
    for (auto &subproblem : subproblems) {
      subproblem->add_results_to_json(json_output);
    }
    std::ofstream o(output_file);
    o << json_output.dump(1, '\t');
  }

  void Problem::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state, nlohmann::json &initial_json) {

    for (auto &subproblem : subproblems) {
      auto type = subproblem->get_type();
      auto initial_json_iterator = initial_json["subproblems"].find(type);
      if (initial_json_iterator == initial_json["subproblems"].end()) {
        gthrow(
            {"Subproblem ", type, " has no initial values in the json files!"});
      }

      auto &subproblem_initial_json = *initial_json_iterator;
      subproblem_initial_json["GRAZER_file_directory"]
          = initial_json["GRAZER_file_directory"];
      aux_json::replace_entry_with_json_from_file(
          subproblem_initial_json, "initial_json");
      subproblem->set_initial_values(
          new_state, subproblem_initial_json["initial_json"]);
    }
  }

  void Problem::set_initial_controls_of_timestep(
      double time, Eigen::Ref<Eigen::VectorXd> controls,
      nlohmann::json const &control_json) {
    // for (auto &subproblem : subproblems) {
    //   auto type = subproblem->get_type();
    //   auto initial_json_iterator = control_json["subproblems"].find(type);
    //   if (initial_json_iterator == initial_json["subproblems"].end()) {
    //     gthrow(
    //         {"Subproblem ", type, " has no initial values in the json
    //         files!"});
    //   }
  }

} // namespace Model
