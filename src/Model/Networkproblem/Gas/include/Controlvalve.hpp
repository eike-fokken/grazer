#pragma once
#include "Control.hpp"
#include "Edge.hpp"
#include "Gasedge.hpp"
#include "Shortcomponent.hpp"
#include <nlohmann/json.hpp>

namespace Model::Networkproblem::Gas {

  class Controlvalve final : public Shortcomponent {
  public:
    static std::string get_type();

    Controlvalve(nlohmann::json const &edge_json,
                 std::vector<std::unique_ptr<Network::Node>> &nodes);

    void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                  double new_time,
                  Eigen::Ref<Eigen::VectorXd const> const &last_state,
                  Eigen::Ref<Eigen::VectorXd const> const &new_state)
        const override;
    void evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                   double last_time, double new_time,
                                   Eigen::Ref<Eigen::VectorXd const> const &,
                                   Eigen::Ref<Eigen::VectorXd const> const
                                       &new_state) const override;

    void set_initial_values(Eigen::Ref<Eigen::VectorXd> new_state,
                            nlohmann::ordered_json initial_json) override;

    void print_to_files(std::filesystem::path const &output_directory) override;

  private:

    Control<1> const control_values;


  };



}
