#pragma once
#include "Shortcomponent.hpp"

namespace Model::Networkproblem::Gas {

  class Shortpipe : public Shortcomponent {

  public:
    static std::string get_type();

    using Shortcomponent::Shortcomponent;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::ordered_json initial_json) final;

    void print_to_files(std::filesystem::path const &output_directory) final;
  };
} // namespace Model::Networkproblem::Gas
